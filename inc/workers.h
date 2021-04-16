#ifndef __IOWORKER__H__
#define __IOWORKER__H__

#include "base.h"
#include "visor_msg.h"
#include "Reactor.h"
#include "EventHandler.h"
#include "networking.h"


#define NB_SCOPE_GROUP  3
#define NB_TASK_WORKERS 8
#define NB_IO_WORKERS   4
//========================================================================
//                      worker scope declarations
//------------------------------------------------------------------------
FtorType(Run,Var,Var args);//virtual function for the event-loops
//------------------------------------------------------------------------
//base class of all worker scopes
Class(WorkerScope,
    f_Run       Run;        //The event loop of the scope
    h_Reactor   reactor;    //The reactor of the worker-scope
    threadID    scopeID;    //The id given to the scope by the visor
    pipeFD      toVisor;    //A non-blocking pipe inwhich to write variables 
                            //  of type<h_VisorMsg> to send to the visor thread
    pipeFD      fromVisor;  //A non-blocking pipe inwhich to read variables
                            //  of type<h_VisorMsg> sent by the visor thread
    pthread_t   id;
);
Class(WorkerScopeInitStruct,
    f_Run       run;
    h_Reactor   reactor;
    pipeFD      toVisor;
    pipeFD      fromVisor;
);
//------------------------------------------------------------------------
//class of the scope of any workers managing 
//communication with the clients
Class(IOWorkerScope,
    WorkerScope     super;              //enherits the base class
    pipeFD          toTaskWorkers;      //A non-blocking pipe inwhich to 
                                        //  write variables of the type <h_EvtHandler> 
                                        //  containing a pt_operation as data member
                                        //  and the associated connection socket FD as 
                                        //  the event-source this acts as the task queue 
                                        //  of the database engine
    pipeFD          fromTaskWorkers;    //A nonblocking pipe inwhich to read variables of 
                                        //  the type<h_EvtHandler> containing a h_Buffer as 
                                        //  the data member and the associated connection's 
                                        //  FD as the event-source. This is the task-completion queue.
);
Class(IOWorkerScopeInitStruct,
    WorkerScopeInitStruct super;
    pipeFD fromTask;
    pipeFD toTask;
);
//------------------------------------------------------------------------
//class of the scope of any workers 
//managing database operations
Class(TaskWorkerScope,
    WorkerScope         super;          //enherits base class
    pipeFD              toIOWorkers;    //A nonblocking pipe inwhich to write variables of 
                                        //  the type<h_EvtHandler> containing 
                                        //  a h_Buffer as the data member and the 
                                        //  associated connection's FD as the event-source. 
                                        //  This is the task-completion queue.
    pipeFD              fromIOWorkers;  //A non-blocking pipe inwhich to 
                                        //  read variables of the type <h_EvtHandler> 
                                        //  containing a pt_operation as data member
                                        //  and the associated connection socket FD as 
                                        //  the event-source this acts as the task queue 
                                        //  of the database engine
    pthread_mutex_t*    mut;
    pthread_rwlock_t*   lock;                            
);
//------------------------------------------------------------------------
//class of the scope of the main thread
Class(VisorScope,
    WorkerScope         super;
    h_Reactor*          reactors[NB_SCOPE_GROUP];//one for each scope implementation
    
    int nbTaskWorkers;
    TaskWorkerScope*    taskWorkers;
    int nbIOWorkers;
    IOWorkerScope*      IOWorkers;

    pthread_mutex_t     mutx;
    pthread_rwlock_t    DBLock;         //A read/write lock in FIFO mode to synchronize 
                                        //  access to the db by the workpool
);
Class(VisorScopeInitStruct,
    WorkerScopeInitStruct   super;
    int                     nbTaskWorkers;
    int                     nbIOWorkers;
    str                     portNUM;
);
//========================================================================


//========================================================================
//                          INIT helpers
//------------------------------------------------------------------------
#define VISOR_REACTOR 0
#define IO_REACTOR 1
#define TASK_REACTOR 2
#define WRITE_END 1
#define READ_END 0
h_VisorScope VisorScopeNew(h_VisorScopeInitStruct attr);
//========================================================================

//========================================================================
//                      polymorphic local instance
//------------------------------------------------------------------------
static __thread  h_WorkerScope Scope;//initialized upon entry by every eventloop
//========================================================================



//========================================================================
//                      eventloops
//------------------------------------------------------------------------
extern Var VisorRun(Var args);
extern Var IOWorkerRun(Var args);
extern Var TaskWorkerRun(Var args);
//========================================================================



#endif  //!__IOWORKER__H__