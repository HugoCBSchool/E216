
#include "EventHandler.h"


h_EvtHandler EvtHandlerNew(){
    h_EvtHandler eh=xcalloc(1,sizeof(EvtHandler));
    return eh;
}
void         EvtHandlerFree(Var obj){
    h_EvtHandler eh=obj;
    
    if(!obj)
        return;
    
    (eh->freeData?eh->freeData:free)(eh->eventData);

    free(eh);
    memset(eh,0,sizeof(EvtHandler));
}
sys_t EvtHandlerHandleEvent(h_Evt event){
    return (
        event->data.ptr
        ?   Cast(EvtHandler,(event->data.ptr))->handleEvent(event)
        :   -1
    );
}
