#include "workers.h"

#define NB_IO_THREAD    4
#define NB_TASK_THREAD  8


int	main(int argc, char **argv){

    VisorScopeInitStruct init={
        .super          ={  .run        =VisorRun   ,
                            .reactor    =NULL       ,/*not used here*/
                            .toVisor    =-1         ,/*not used here*/
                            .fromVisor  =-1         }/*not used here*/
    ,   .portNUM        =PORT_NUM,
        .nbIOWorkers    =NB_IO_THREAD,
        .nbTaskWorkers  =NB_TASK_THREAD
    };
    Scope=((h_WorkerScope)VisorScopeNew(&init));

    Scope->Run(Scope);

    return 0;
}
