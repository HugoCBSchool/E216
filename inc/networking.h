#ifndef __NETWORKING__H__
#define __NETWORKING__H__

#include "base.h"

#define PORT_NUM        "8888"
#define SERVER_IP       "127.0.0.1"

sys_t  SocketSetNonblocking(sockFD sock);
sockFD SocketCreateAndBindAndListen(const str portNUM);
sockFD SocketConnect(const str hostIP, const str portNUM);

#endif  //!__NETWORKING__H__