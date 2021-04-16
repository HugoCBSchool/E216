#include "workers.h"
//====================================================================
//              inliners of this module
//--------------------------------------------------------------------
inline void WorkerInit(h_WorkerScope scope){
    Scope=Cast(WorkerScope,scope);
    Scope->id=pthread_self();
    return;
}
inline void terminate(exitvalue){
    pthread_detach(pthread_self());
    pthread_exit(0);
}
inline void notifyVisor(VisorMsgType type){
    h_VisorMsg msg=VisorMsgNew(
        type,
        Scope,
        Scope
    );
    write(Scope->toVisor,&msg,sizeof(h_VisorMsg));
}
h_VisorScope VisorScopeNew(h_VisorScopeInitStruct attr){
    h_VisorScope nouv=xcalloc(1,sizeof(VisorScope));
    pipeFD                      IO2Task[2];
    pipeFD                      Task2IO[2];
    pipeFD                      worker2visor[2];
    pipeFD                      visor2worker[2];
    IOWorkerScopeInitStruct     initio;
    h_WorkerScope               sup=&nouv->super;
    h_EvtHandler                handler;
    sockFD                      acceptFD;


    if((acceptFD=SocketCreateAndBindAndListen(attr->portNUM))<=0)
        perror("open server socket");
        abort();

    //transfer flat data
    sup->scopeID=0;
    sup->Run    =VisorRun;
    nouv->nbIOWorkers   =attr->nbIOWorkers;
    nouv->nbTaskWorkers =attr->nbTaskWorkers;

    //create the reactors
    for(int i=0;i<NB_SCOPE_GROUP;++i){
        nouv->reactors[i]=ReactorNew();
    }


    {   //open the event queues
        if(pipe2(IO2Task,O_NONBLOCK)<0)
            perror("pipe()");
            abort();
        if(pipe2(Task2IO,O_NONBLOCK)<0)
            perror("pipe()");
            abort();
        if(pipe2(worker2visor,O_NONBLOCK)<0)
            perror("pipe()");
            abort();
        if(pipe2(visor2worker,O_NONBLOCK)<0)
            perror("pipe()");
            abort();
    }


    {   //init the visor

        //setup lock and mutx
        pthread_rwlockattr_t attr;
        if(pthread_rwlockattr_init(&attr)!=0)
            perror("pthread_attr_init");
            abort();
        if(pthread_rwlockattr_setkind_np(&attr,
            PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP)!=0)
            perror("pthread_rwlockattr_setkind_np");
            abort();
        
        if(pthread_mutex_init(&nouv->mutx,NULL)!=0)
            perror("pthread_mutex_init");
            abort();
        
        if(pthread_rwlock_init(&nouv->DBLock,&attr)!=0)
            perror("pthread_rwlock_init");
            abort();

        //setup pipes
        sup->reactor    =nouv->reactors[VISOR_REACTOR];
        sup->fromVisor  =visor2worker[WRITE_END];
        sup->toVisor    =worker2visor[READ_END];

        //register message queue notif
        handler=EvtHandlerNew();
        handler->eventData      =NULL;
        handler->eventSource    =sup->toVisor;
        handler->mode           =NOTIFY_ALWAYS(EPOLLIN);
        handler->freeData       =NULL;
        handler->handleEvent    =handleVisorMsg;
        ReactorRegister(sup->reactor,handler);

    }


    {   //prepare the workgroup of IO
        nouv->IOWorkers=xcalloc(attr->nbIOWorkers,sizeof(IOWorkerScope));
        initio.super.run        =IOWorkerRun;
        initio.toTask           =IO2Task[WRITE_END];
        initio.super.toVisor    =worker2visor[WRITE_END];


        {   //register the read-ends of the workgroup pipes
            initio.super.reactor=nouv->reactors[IO_REACTOR];

            {   //register the visor msg handler
                initio.super.fromVisor=visor2worker[READ_END];
                    handler=EvtHandlerNew();
                    handler->eventData=NULL;
                    handler->eventSource=initio.super.fromVisor;
                    handler->freeData=NULL;
                    handler->handleEvent=handleVisorMsg;
                    handler->mode=NOTIFY_ALWAYS(EPOLLIN);
                ReactorRegister(initio.super.reactor,handler);
            }

            {  //register the task completion handler
                initio.fromTask=Task2IO[READ_END];
                    handler=EvtHandlerNew();
                    handler->eventData=NULL;
                    handler->eventSource=initio.fromTask;
                    handler->freeData=NULL;
                    handler->handleEvent=handleTaskCompletion;
                    handler->mode=NOTIFY_ALWAYS(EPOLLIN);
                ReactorRegister(initio.super.reactor,handler);
            }

            {  //register the IO accept handler
                handler=EvtHandlerNew();
                handler->eventData=NULL;
                handler->eventSource=acceptFD;
                handler->freeData=NULL;
                handler->handleEvent=handleAccept;
                handler->mode=NOTIFY_ONCE(EPOLLIN);
                ReactorRegister(initio.super.reactor,handler);
            }

        }

        for(int i=0;i<attr->nbIOWorkers;++i){
            h_IOWorkerScope cur=&nouv->IOWorkers[i];

            cur->fromTaskWorkers=initio.fromTask;
            cur->toTaskWorkers  =initio.toTask;
            Cast(WorkerScope,cur)->fromVisor=initio.super.fromVisor;
            Cast(WorkerScope,cur)->reactor  =initio.super.reactor;
            Cast(WorkerScope,cur)->Run      =initio.super.run;
            Cast(WorkerScope,cur)->scopeID  =ThreadIndex2ID(i+1);
            Cast(WorkerScope,cur)->toVisor  =initio.super.toVisor;
        }
    }

    {   //prepare the task workgroup
        nouv->nbTaskWorkers=xcalloc(attr->nbIOWorkers,sizeof(TaskWorkerScope));
        {
            {   //init the reactor
                handler=EvtHandlerNew();
                handler->handleEvent=handleVisorMsg;
                handler->eventSource=visor2worker[READ_END];
                handler->eventData=NULL;
                handler->freeData=NULL;
                handler->mode=NOTIFY_ALWAYS(EPOLLIN);
                ReactorRegister(nouv->reactors[TASK_REACTOR],handler);

                handler=EvtHandlerNew();
                handler->handleEvent=handleTaskReception;
                handler->eventSource=IO2Task[READ_END];
                handler->eventData=NULL;
                handler->freeData=NULL;
                handler->mode=NOTIFY_ALWAYS(EPOLLIN);
                ReactorRegister(nouv->reactors[TASK_REACTOR],handler);
                
            }
            {   //setup the workgroup
                for(int i=0;i<attr->nbTaskWorkers;++i){
                    h_TaskWorkerScope cur=&nouv->taskWorkers[i];
                    h_WorkerScope     sup=&nouv->taskWorkers[i].super;

                    cur->fromIOWorkers  =IO2Task[READ_END];
                    cur->toIOWorkers    =Task2IO[WRITE_END];
                    cur->lock           =&nouv->DBLock;
                    cur->mut            =&nouv->mutx;

                    sup->fromVisor  =visor2worker[READ_END];
                    sup->toVisor    =worker2visor[WRITE_END];
                    sup->reactor    =nouv->reactors[TASK_REACTOR];
                    sup->Run        =TaskWorkerRun;
                    sup->scopeID    =ThreadIndex2ID(1+attr->nbIOWorkers+i);
                }
            }
        }


    }

    return nouv;
}

//====================================================================


//====================================================================
//                  Event loop implementations
//--------------------------------------------------------------------
extern Var  IOWorkerRun(Var args){
    ReactorStatus   status;

    WorkerInit(Cast(WorkerScope,args));
     
    while(1){
        switch(status=ReactorRun(Scope->reactor)){
            IO_ERROR:
                printf("IO error on thread{%d}\n",Scope->scopeID);
                break;
            REACTOR_ERROR:
                printf("Reactor error/interupt on thread{%d}\n",Scope->scopeID);
                goto _terminate;
            REACTOR_INTERUPT:
                printf("Reactor Interupt on thread{%d}\n",Scope->scopeID);
                goto _terminate;
            default:
                printf("Unknown error on thread{%d}\n",Scope->scopeID);
                goto _terminate;
        }
    }

    _terminate:
    notifyVisor(HAS_TERMINATED);
    terminate(1);
    pthread_exit(1);
}
extern Var  TaskWorkerRun(Var args){
    ReactorStatus status;

    WorkerInit(Cast(WorkerScope,args));

    while(1){
        switch(status=ReactorRun(Scope->reactor)){
            IO_ERROR:
                printf("IO error on thread{%d}\n",Scope->scopeID);
                break;
            REACTOR_ERROR:
                printf("Reactor error/interupt on thread{%d}\n",Scope->scopeID);
                goto _terminate;
            REACTOR_INTERUPT:
                printf("Reactor Interupt on thread{%d}\n",Scope->scopeID);
                goto _terminate;
            default:
                printf("Unknown error on thread{%d}\n",Scope->scopeID);
                goto _terminate;
        }
    }
    _terminate:
    notifyVisor(HAS_TERMINATED);
    terminate(1);
    pthread_exit(1);
}
extern Var VisorRun(Var args){
    h_VisorScope self=Cast(VisorScope,args);
    WorkerInit(Cast(WorkerScope,args));
    ReactorStatus status;

    pthread_t tid;
    for(int i=0;i<self->nbTaskWorkers;++i){
        if((pthread_create(
            &tid,NULL,self->taskWorkers[i].super.Run,&self->taskWorkers[i]
        ))!=0)
            perror("create()");
            abort();
    }
    for(int i=0;i<self->nbTaskWorkers;++i){
        if((pthread_create(
            &tid,NULL,self->IOWorkers[i].super.Run,&self->IOWorkers[i]
        ))!=0)
            perror("create()");
            abort();
    }

    while(1){
        switch(status=ReactorRun(Scope->reactor)){
            IO_ERROR:
                printf("IO error on main thread{%d}\n",Scope->scopeID);
                break;
            REACTOR_ERROR:
                printf("Reactor error/interupt on main thread{%d}\n",Scope->scopeID);
                goto _terminate;
            REACTOR_INTERUPT:
                printf("Reactor Interupt on main thread{%d}\n",Scope->scopeID);
                goto _terminate;
            default:
                printf("Unknown error on main thread{%d}\n",Scope->scopeID);
                goto _terminate;
        }
    }
    _terminate:
    for(int i=0;i<self->nbTaskWorkers;++i)
        pthread_cancel(self->taskWorkers[i].super.id);
    for(int i=0;i<self->nbIOWorkers;++i)
        pthread_cancel(self->IOWorkers[i].super.id);
    
    exit(1);//should never die, so...

}
//====================================================================
