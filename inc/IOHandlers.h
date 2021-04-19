#ifndef __IOHANDLERS__H__
#define __IOHANDLERS__H__

#include "base.h"
#include "ReactorCommon.h"
#include "buffer.h"
#include "header.h"
#include "EventHandler.h"
#include "Reactor.h"
#include "client.h"
#include "workers.h"
#include "networking.h"
#include "parse.h"
typedef enum _handlerStatus{
    E_TERMINATE      =-6,
    E_WRONG_HANDLER2 =-5,
    E_IO_ERROR2      =-4,
    E_SERIALIZATION  =-3,
    E_WRONG_HANDLER  =-2,
    E_IO_ERROR       =-1,
    HANDLER_SUCCESS  =0 
}HandlerStatus;

extern sys_t handleAccept(          h_Evt evt);
//handles the reception of the header of the request
extern sys_t handleInit(            h_Evt evt);

//handles the reception of the body of the request
extern sys_t handleRead(            h_Evt evt);

//handles the transmission of the response of the request
extern sys_t handleWrite(           h_Evt evt);

//handles the completion of the request
extern sys_t handleTaskCompletion(  h_Evt evt);

extern sys_t handleVisorMsg(h_Evt evt);

extern sys_t handleTaskReception(h_Evt evt);



#endif  //!__IOHANDLERS__H__