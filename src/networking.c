#include "networking.h"


sys_t  SocketSetNonblocking(sockFD sock){
    sys_t flags, result;

    flags = fcntl(sock, F_GETFL, 0);

    if(flags == -1){
        perror("fcntl");
        return -1;
    }

    if((result = fcntl(sock,F_SETFL,flags|O_NONBLOCK))==-1){
        perror("fcntl");
        return -1;
    }

    return 0;
}
sockFD SocketCreateAndBindAndListen(const str portNUM){
    sockFD              sock    =socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in  server;
    
    if(sock<=0)
        perror("socker()");
        return -1;
    
    if(setsockopt(sock,SOL_SOCKET,(SO_REUSEPORT|SO_REUSEADDR),&(int){1},sizeof(int))<0)
        perror("SO_REUSEADDR");
        close(sock);
        return -1;

    bzero(&server,sizeof(server));
    server.sin_family       =AF_INET;
    server.sin_addr.s_addr  =INADDR_ANY;
    server.sin_port         =htons(atoi(portNUM));

    if((bind(sock,(struct sockaddr*)&server,sizeof(server)))<0)
        perror("bind()");
        close(sock);
        return -1;

    if(listen(sock,SOMAXCONN)<0)
        perror("listen()");
        close(sock);
        return -1;

    return sock;
}
sockFD SocketConnect(const str hostIP, const str portNUM){
    int                 portNum     =atoi(portNUM);
    sockFD              sock        =socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in  serveraddr;


    if (sock<0)
        perror("ERROR opening socket");


    bzero((str) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family       =AF_INET;
    serveraddr.sin_addr.s_addr  =inet_addr(hostIP);
    serveraddr.sin_port         =htons(portNum);

    /* connect: create a connection with the server */
    if(connect(sock,(const struct sockaddr*)&serveraddr,sizeof(serveraddr))<0){
        perror("Connection error");
        return -1;
    }
    return sock;
}
