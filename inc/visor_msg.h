#ifndef __VISOR_MSG__H__
#define __VISOR_MSG__H__

#include "base.h"

//========================================================================
//                 thread-group identification macros
//------------------------------------------------------------------------
Type(threadID,int);
Type(threadGroup,int);
#   define inGroup(id,group)                ((id&group)!=0)
#   define ThreadIndex2ID(index)            (1<<(index-1))
//========================================================================

//========================================================================
//                 messaging reflection interface
//------------------------------------------------------------------------
typedef enum _VisorMessageType{
    HAS_TERMINATED,
    TERMINATE_WORKER
}VisorMsgType;
//========================================================================


//========================================================================
//                 message data structure
//------------------------------------------------------------------------
Class(VisorMsg,
    VisorMsgType    msgType;
    threadID        scopeID;
    Var             msgData;
);
//========================================================================

//========================================================================
//                 message interface
//------------------------------------------------------------------------
h_VisorMsg VisorMsgNew(VisorMsgType type,threadID scopeID,Var data);
void       VisorMsgFree(Var obj);
//========================================================================



#endif  //!__VISOR_MSG__H__