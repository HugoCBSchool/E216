#ifndef __REACTORCOMMON__H__
#define __REACTORCOMMON__H__

#include "base.h"


FtorType(handleEvent,sys_t  ,h_Evt  event);
FtorType(free       ,void   ,Var    obj);
Type(todo,int);
Type(epollFD,int);

ClassDecl(EvtHandler);
Class(Reactor,
    epollFD epollfd;
);
ClassDef(EvtHandler,
    sockFD              eventSource;
    ASIO_MODE           mode;
    f_handleEvent       handleEvent;
    Var                 eventData;
    f_free              freeData;
);



#endif