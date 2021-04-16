#ifndef __NETWORKING__H__
#define __NETWORKING__H__

#include "base.h"


sys_t  SocketSetNonblocking(sockFD sock);
sockFD SocketCreateAndBindAndListen(const str portNUM);
sockFD SocketConnect(const str hostIP, const str portNUM);

#endif  //!__NETWORKING__H__