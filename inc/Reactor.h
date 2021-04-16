#ifndef __REACTOR__H__
#define __REACTOR__H__

#include "base.h"
#include "ReactorCommon.h"
#include "EventHandler.h"
#include "IOHandlers.h"
#include "workers.h"

typedef enum _ReactorStatusFlag{
    REACTOR_OK          ,
    REACTOR_FATAL_ERROR ,
    REACTOR_IO_ERROR    ,
    REACTOR_INTERUPT
}ReactorStatus;

//creates a new empty reactor
h_Reactor     ReactorNew();

//deletes a reactor and cleans associated ressources
void          ReactorFree(Var obj);

//registers given event for notification.
//the fields of the handler are used to tell the reactor which kind of
//events are expected
void          ReactorRegister(  h_Reactor self, h_EvtHandler handler);

//unregisters the given event provided by the handler from 
//the notification list. No file descriptors and no handler data are 
//destroyed/removed. These actions must be done by the client of the reactor.
void          ReactorUnregister(h_Reactor self, h_EvtHandler handler);

//starts the reaction loop in the current thread.
//this function segfaults if the <Scope> thread-local variable is not yet 
//initialized properly by the workers.h module.
ReactorStatus ReactorRun(h_Reactor self);







#endif  //!__REACTOR__H__