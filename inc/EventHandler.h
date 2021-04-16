#ifndef __EVENTHANDLER__H__
#define __EVENTHANDLER__H__

#include "base.h"
#include "ReactorCommon.h"

#define NOTIFY_ONCE(x)     (EPOLLONESHOT|EPOLLET|x)
#define NOTIFY_ALWAYS(x)   ((~(EPOLLONESHOT|EPOLLET))&x)


h_EvtHandler    EvtHandlerNew();
void            EvtHandlerFree(Var obj);
sys_t           EvtHandlerHandleEvent(h_Evt event);




#endif  //!__EVENTHANDLER__H__