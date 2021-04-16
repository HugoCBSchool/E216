#include "Reactor.h"

#define MAX_EVENTS  64
#define FOREVER     -1

h_Reactor ReactorNew(){

    h_Reactor reactor=xcalloc(1,sizeof(Reactor));

    if((reactor->epollfd=epoll_create1(0))<0){
        perrno("%s","Reactor new error.");
        exit(-1);
    }
    return reactor;
}
void ReactorFree(Var obj){
    h_Reactor reactor=Cast(Reactor,obj);
    
    close(epollfd);
    free(reactor);
}
void ReactorRegister( h_Reactor self,h_EvtHandler handler){
    Evt event={
        .data.ptr=handler,
        .events=handler->mode
    };
    
    //can fail sometimes
    if(epoll_ctl(self->epollfd,handler->eventSource,EPOLL_CTL_ADD,&event)==0)
        goto _success;
    //can fail sometimes
    if(epoll_ctl(self->epollfd,handler->eventSource,EPOLL_CTL_MOD,&event)==0)
        goto _success;

    //never fails, just abort on error
    if(epoll_ctl(self->epollfd,handler->eventSource,EPOLL_CTL_DEL,&event)!=0){
        perrno("%s","Failed to register in reactor. EPOLL_CTL_DEL\n");
        exit(-1);
    }
    if(epoll_ctl(self->epollfd,handler->eventSource,EPOLL_CTL_ADD,&event)!=0){
        perrno("%s","Failed to register in reactor. EPOLL_CTL_ADD\n");
        exit(-1);
    }
    
    _success:
}
void ReactorUnregister(h_Reactor self,h_EvtHandler handler){
    Evt event;//for legacy 2.6.1
    
    epoll_ctl(self->epollfd,EPOLL_CTL_DEL,handler->eventSource,event);
    //no error check since we might have some conditions where a fd 
    //could be unregistered more than once
}
ReactorStatus ReactorRun(h_Reactor self){
    Evt             events[MAX_EVENTS];
    int             nevents=0;
    ReactorStatus   status=REACTOR_OK;
    int             crash=0;
    int             terminate=0;

    _again:switch(
        nevents=epoll_wait(self->epollfd,events,MAX_EVENTS,FOREVER)
    ){
        case -1:    
            return REACTOR_IO_ERROR;

        case 0:
            goto _again;

        default:
            break;
    }
    _handle_events:{
        for(int i=0;i<nevents;++i){
            switch(EvtHandlerHandleEvent(&events[i])){
                case HANDLER_SUCCESS:
                    break;
                case E_TERMINATE:
                    printf(
                        "Reactor: Termination request received and acknowledged "
                        "by thread{%d}\n",
                        Scope->scopeID
                    );
                    terminate=1;
                    break;
                case E_IO_ERROR:
                    printf(
                        "Reactor: IO error of type 1 "
                        "for event of number{%d} by thread{%d}\n",
                        events[i].events,
                        Scope->scopeID
                    );
                    crash=1;
                    break;
                case E_IO_ERROR2:
                    printf(
                        "Reactor: IO error of type 2"
                        "for event of number{%d} by thread{%d}\n",
                        events[i].events,
                        Scope->scopeID
                    );
                    crash=1;
                    break;
                case E_WRONG_HANDLER:
                    printf(
                        "Reactor: wrong handler error of type 1 "
                        "for event of number{%d} by thread{%d}\n",
                        events[i].events,
                        Scope->scopeID
                    );
                    crash=1;
                    break;
                case E_WRONG_HANDLER2:{
                    printf(
                        "Reactor: wrong handler error of type 2 "
                        "for event of number{%d} by thread{%d}\n",
                        events[i].events,
                        Scope->scopeID
                    );
                    crash=1;
                    break;
                }
                case E_SERIALIZATION:{
                    printf(
                        "Reactor: serialization error "
                        "for event of number{%d} by thread{%d}\n",
                        events[i].events,
                        Scope->scopeID
                    );
                    crash=1;
                    break;
                }
                default:{
                    printf(
                        "Reactor: unknown error "
                        "for event of number{%d} by thread{%d}\n",
                        events[i].events,
                        Scope->scopeID
                    );
                    crash=1;
                    break;
                }
            }
        }
        if(crash)
            return REACTOR_FATAL_ERROR;
        if(terminate)
            return REACTOR_INTERUPT;

        goto _again;
    }
    return REACTOR_FATAL_ERROR;//never here
}

