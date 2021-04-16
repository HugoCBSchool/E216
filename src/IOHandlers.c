#include "IOHandlers.h"

//these will all segfault if runned in the context of any other than a IOWorker thread
Class(ListenData,
    struct sockaddr socketAddress;
    int addrLen;
);
//handlers associated with the IOreactor
extern sys_t handleAccept(          h_Evt evt){
    h_EvtHandler        handler=Cast(EvtHandler,evt->data.ptr);
    h_EvtHandler        newcon =NULL;
    struct sockaddr_in  client;
    int                 cxlen;
    Evt                 stub={0};

    if(!(evt->events&handler->mode))
        return E_WRONG_HANDLER;
    
    bzero(&client,sizeof(struct sockaddr_in));
    cxlen=sizeof(client);
    
    switch(commfd=accept4(
        handler->eventSource,
        &client,&cxlen
        SOCK_NONBLOCK
    )){
        case -1: 
            if(!(errno&(EAGAIN|EWOULDBLOCK)))
                return E_IO_ERROR;
        case 0:
            ReactorRegister(Scope->reactor,handler);
            return HANDLER_SUCCESS;
        default:
            break;
    }

    newcon=EvtHandlerNew();
    newcon->eventData   =NULL;
    newcon->eventSource =commfd;
    newcon->freeData    =NULL;
    newcon->handleEvent =handleInit;
    newcon->mode        =NOTIFY_ONCE(EPOLLIN);
    ReactorRegister(Scope->reactor,handler);

    stub.events     =NOTIFY_ONCE(EPOLLIN);
    stub.data.ptr   =newcon;
    
    return handleInit(&stub);
}
extern sys_t handleInit( h_Evt evt){
    h_EvtHandler handler=evt->data.ptr;
    h_Buffer     buf;
    h_CommHeader header;
    sys_t        retval;

    if( !(  evt->events & handler->mode ) ){
        errno=EINVAL;
        return E_WRONG_HANDLER;//must tell epoll to notify the reactor manager
    }

    {   //init buffer conditionnaly to its existance
        buf=(
            handler->eventData
            ?   handler->eventData
            :   (handler->eventData=BufferNew(sizeof(CommHeader)))
        );
        handler->freeData=BufferFree;
    }

    //read from the socket
    switch(BufferIO_recv(buf,handler->eventSource)){
        case 0://if done
            break;

        case -1://if error
            if( !errno&(EAGAIN|EWOULDBLOCK) )
                return E_IO_ERROR;//the reactor will choose wether to register again

        default://if not done and not fatal error
            //register again for the same callback/event
            handler->mode=NOTIFY_ONCE(EPOLLIN);
            ReactorReRegister(ioScope->ioreactor,handler);

            return HANDLER_SUCCESS;
    }
    
    //take the header, init using the size gotten with it
    CommHeaderMarshall(header=BufferMove(buf));
    BufferInit(buf,header->size);
    free(header);
    

    //set notification with the next callback
    handler->handleEvent=handleRead;
    evt->events=(handler->mode=NOTIFY_ONCE(EPOLLIN));

    //to ensure that we completely empty the kernel-buffer before registering
    return handleRead(evt);
}
extern sys_t handleRead( h_Evt evt){
    h_EvtHandler handler=evt->data.ptr;
    h_Buffer     buf    =handler->eventData;
    char*        serial;
    pt_operation op;
    int          nbytes;

    if( !(  evt->events & handler->mode ) ){
        errno=EINVAL;
        return E_WRONG_HANDLER;//must tell epoll to notify the reactor manager
    }

    //read from the socket
    switch(BufferIO_recv(buf,handler->eventSource)){
        case 0://if done
            break;
        case -1://if error
            if( errno != (EAGAIN|EWOULDBLOCK) )
                return E_IO_ERROR;//the reactor himself will decide wether to register again or not
        default://if not done and not fatal error
            //register again for the same callback/event
            ReactorRegister(ioScope->ioreactor,handler);
            return HANDLER_SUCCESS;
    }

    {   //deserialize and setup the handler for further processing
        if(!(op=deserialiser_operation(serial=BufferMove(buf)))){
            free(serial);
            errno=ENOMEM;
            return E_SERIALIZATION;
        }

        //populate the handler with the new content in order to prepare 
        //for reactor-space context change
        BufferFree(buf);
        free(serial);
        handler->eventData  =op;
        handler->freeData   =operation_liberer;
        handler->eventSource=handler->eventSource;//must remember where to respond
        handler->handleEvent=NULL;                //make sure nothing funky
        handler->mode       =0;
    }

    //send the task to task handlers
    switch(nbytes=write(ioScope->toTaskWorkers,&handler,sizeof(h_EvtHandler))){
        case -1:
            if(!errno&(EAGAIN|EWOULDBLOCK))
                return E_IO_ERROR;

        case 0:
            //means that we exceded 16KB worth of pointers (16K concurrent requests)
            errno=ECOMM;
            return E_IO_ERROR;

        default:
            //will always mean that completely sent because of atomic write behaviour
            return HANDLER_SUCCESS;
    }
    
    return HANDLER_SUCCESS;
}
extern sys_t handleWrite(h_Evt evt){
    h_EvtHandler handler    =evt->data.ptr;
    h_Buffer     buf        =handler->eventData;
    ssize_t      bytes_left;

    if(!(evt->events&handler->mode)){
        errno=EINVAL;
        return E_WRONG_HANDLER;
    }

    switch((bytes_left=BufferIO_send(buf,handler->eventSource))){
        case 0:
            break;//done sending

        case -1://problem
            if(!(errno&(EWOULDBLOCK|EAGAIN)))
                return E_IO_ERROR;

        default://default is to register again
            handler->mode=NOTIFY_ONCE(EPOLLOUT);
            ReactorRegister(handler->reactor,handler);

            return HANDLER_SUCCESS;
    }

    {   //cleanup
        ReactorUnregister(handler->reactor,handler);
        EvtHandlerFree(handler);
        evt->data.ptr=NULL;//ensures the reactor doesnt try something stupid
    }
    return HANDLER_SUCCESS;
}
extern sys_t handleTaskCompletion(h_Evt evt){
    h_EvtHandler    handler=evt->data.ptr;
    h_EvtHandler    recieved;
    h_Buffer        recbuf=NULL;
    int             interupt_raised=1;
    sys_t           nb_elem;
    sys_t           exitvalue=HANDLER_SUCCESS;

    if(!(evt->events & handler->mode)){
        errno=EINVAL;
        exitvalue=E_WRONG_HANDLER2;
        goto _register;
    }

    switch((nb_elem=read(handler->eventSource,&recieved,sizeof(h_EvtHandler)))){

        case -1://check if it is a false positive
            if(!(errno&(EWOULDBLOCK|EAGAIN)))
                return E_IO_ERROR2;

        case 0:
            return HANDLER_SUCCESS;//nothing to process currently

        default:
            //setup the reactor-specific context attributes
            recieved->mode       =NOTIFY_ONCE(EPOLLOUT);
            recieved->reactor    =ioScope->ioreactor;
            recieved->handleEvent=handleWrite;         

            Evt dummy={.events=NOTIFY_ONCE(EPOLLOUT),.data.ptr=recieved};

            return handleWrite(dummy);//this will take care of registration
    }

    return HANDLER_SUCCESS;//never here
}
extern sys_t handleVisorMsg(h_Evt evt){
    h_EvtHandler handler=evt->data.ptr;
    h_VisorMsg   msg;
    size_t      nbytes;
    sys_t       exitval;

    if(!(evt->events&handler->mode))
        errno=EINVAL;
        return E_WRONG_HANDLER;
    
    switch(nbytes=read(handler->eventSource,&msg,sizeof(h_VisorMsg))){
        case -1:
            if(!(errno&(EAGAIN|EWOULDBLOCK)))
                return E_IO_ERROR;
        case 0:
            return HANDLER_SUCCESS;
        default:
            break;
    }
    if(inGroup(Scope->scopeID,msg->scopeID))
        exitval=E_TERMINATE;
    VisorMsgFree(msg);

    return exitval;
}
extern sys_t handleTaskReception(h_Evt evt){
    
}