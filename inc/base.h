#ifndef __BASE__H__
#define __BASE__H__

#define _GNU_SOURCE 
#define __USE_GNU

#include <pthread.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#ifndef DBG_PIPE
#define DBG_PIPE stdout
#endif

/**
 * @brief  
 * @note   
 * @retval 
 */
#define ptimestamped(str,...){\
    time_t time=time(NULL);\
    struct tm* time_tm=gmtime(&time);\
    fprintf(\
		DBG_PIPE,\
		"[%d:%d:%d] "str,\
		time_tm->tm_hour,\
		time_tm->tm_min,\
		time_tm->tm_sec,\
		__VA_ARGS__\
	);\
}
//------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @retval 
 */
#define perrno(_str,...) \
    fprintf(DBG_PIPE,"{ errno  :  %d  :  %s }  " _str ,errno,strerror(errno),__VA_ARGS__)
//------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @retval 
 */
#define perrno_if_not_true(cnd,_str,...) if(cnd!=TRUE){\
    fprintf(DBG_PIPE,"{ errno  :  %d  :  %s }  "_str,errno,strerror(errno),__VA_ARGS__);\
}
//------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @retval 
 */
#define perror_die(_str,...) {\
    time_t tim=time(NULL);\
    struct tm* time_tm=gmtime(&tim);\
    fprintf(DBG_PIPE,"[%d:%d:%d]-{ %d  :  %s }  "_str"\n",\
        time_tm->tm_hour,\
        time_tm->tm_min,\
        time_tm->tm_sec,\
        errno,\
        strerror(errno),\
        __VA_ARGS__\
    );\
    exit(EXIT_FAILURE);\
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//----------------------------------------------------------------------------------------
#define _CAT0()                                    //implement CAT for 0 arg 
#define _CAT1(n1)                n1                //implement CAT for 1 arg
#define _CAT2(n1,n2)             n1 ## n2          //implement CAT for 2 args

//------------------------------------------------------
#define CAT2(n1,n2)              _CAT2(n1,n2) 
#define CAT3(n1,n2,n3)           CAT2(n1,CAT2(n2,n3))
#define CAT4(n1,n2,n3,n4)        CAT2(n1,CAT3(n2,n3,n4))
#define CAT5(n1,n2,n3,n4,n5)     CAT2(n1,CAT4(n2,n3,n4,n5))

#define EXPAND(...)                     __VA_ARGS__
#define EXP(x)                          x

#define DEPAREN(X)                      ESC(ISH X)
#   define ISH(...)                        ISH __VA_ARGS__
#   define ESC(...)                        ESC_(__VA_ARGS__)
#   define ESC_(...)                       VAN ## __VA_ARGS__
#   define VANISH
//----------------------------------------------------------------------------------------

#define Type(name,is)                   typedef is name
#define Handle(class)                   CAT2(h_,class)
#define Ftor(name)                      CAT2(f_,name)
#define Member(name)                    CAT2(m_,name)
#define CCast(type,obj)                  ((type)obj)
#define Cast(class,obj)                 ((Handle(class))obj)


Type(Var,void*);

 
//----------------------------------------------------------------------------------------
#define _MethodImpl(class,name)         CAT3(class,_,name)
#define _call(name,...)                 name(__VA_ARGS__)
#define _MethodDecl(class,ret_type,name,...)\
            ret_type name(Handle(class) self , __VA_ARGS__)
#define _MethodDecl_NoArgs(class,ret_type,name,...)\
            ret_type name(Handle(class) self)
#define _FunctionDecl(name,...)\
            name(__VA_ARGS__)
#define _VMethodDef(class,ret_type,name,...)\
            static _MethodDecl(class,ret_type,name,__VA_ARGS)
#define _VMethodDef_NoArgs(class,ret_type,name,...)\
            static _MethodDecl_NoArgs(class,ret_type,name,__VA_ARGS)
//------------------------------------------------------------------------------------
#define FtorType(name,ret_type,...)       typedef ret_type (*Ftor(name))(__VA_ARGS__)
#define MethodType(name,ret_type,...)     FtorType(name,ret_type,)
#define Method(method)                    Member(method)
//-----------------------------------------------

#define VMethodDecl(name)\
            Ftor(name) Member(name)

#       define Call_VM(class,object,method,args)\
                    _call(object.Method(method),&object,DEPAREN(args))

#       define Call_VM0(class,object,method,args)\
                    _call(object.Method(method),&object)

#       define HCall_VM(object,method,args)\
                    _call(object->Method(method),object,DEPAREN(args))

#       define HCall_VM0(class,object,method,args)\
                    _call(object->Method(method),object)


#define VFunctionDecl(name)\
            Ftor(name) Member(name)

#       define  Call_VF(object,func,args)\
                    _call(object.func,DEPAREN(args))

#       define HCall_VF(class,object,func,args)\
                    _call(object->func,DEPAREN(args))


#define MethodDecl(class,ret,name,args)\
            _MethodDecl(class,ret,CAT3(class,_,name),DEPAREN(args))
#define MethodDecl_NoArgs(class,ret,name,args)\
            _MethodDecl_NoArgs(class,ret,CAT3(class,_,name),DEPAREN(args))

#       define Call_M(class,object,method,args)\
                    _call(CAT3(class,_,method),&object,DEPAREN(args))

#       define Call_M0(class,object,method,args)\
                    _call(CAT3(class,_,method),&object)

#       define HCall_M(class,object,method,args)\
                    _call(CAT3(class,_,method),object,DEPAREN(args))

#       define HCall_M0(class,object,method,args)\
                    _call(CAT3(class,_,method),object)


#define FunctionDecl(class,ret,name,args)\
            ret _FunctionDecl(CAT3(class,_,name),DEPAREN(args))

#       define Call_F(class,name,args)\
                    _call(CAT3(class,_,name),DEPAREN(args))


//---------------------------------
#define VMethodDef(class,ret,name,args)\
            _VMethodDef(class,ret,CAT3(class,_,name),DEPAREN(args))

#define VMethodDef_NoArgs(class,ret,name,args)\
            _VMethodDef_NoArgs(class,ret,CAT3(class,_,name),)

#define MethodDef_NoArgs(class,ret,name,args)\
            _MethodDecl_NoArgs(class,ret,name,)

#define MethodDef(class,ret,name,args)\
            _MethodDecl(class,ret,name,DEPAREN(args))

#define FunctionDef(class,ret,name,args) \
            ret _FunctionDecl(CAT3(class,_,name),DEPAREN(args))

//----------------------------------------------------------------------------------------
#define _ClassDef(name,implementation_scope)\
    struct name{                            \
        implementation_scope                \
    }
//---------------------------------------------
#define ClassDecl(name)\
            typedef struct CAT2(_,name) name,*Handle(name)

#define ClassDef(name,implementation_scope)             \
            _ClassDef(CAT2(_,name),implementation_scope)

#define Class(name,implementation_scope)                \
            ClassDecl(name);                            \
            ClassDef(name,implementation_scope)

//----------------------------------------------------------------------------------------
#define on_alloc_error(dest,size) if(!(dest=malloc(size)))
#define on_return(call,op,value)  if(call op value)
#define sys_check(call)           if((call)==-1)
#define sys_check0(call)          if((call)!=0)    
#define cleanup(stack_id)         CAT2(_err_cleanup_,stack_id)
//----------------------------------------------------------------------------------------


#define MutexScope(mutx,scope)                      \
    do{                                             \
        sys_check(pthread_mutex_lock(&mutx)){       \
            printf("mutex scope unable to lock");   \
        }else{                                      \
            scope                                   \
            pthread_mutex_unlock(&mutx);            \
        }                                           \
    }while(0)
    
typedef int     sys_t;
typedef int     nbyte_t;
typedef void*   Var;
typedef int     fd_t;
typedef char*   str;
typedef int     ConnFD;
typedef int     sockFD;
typedef int     pipeFD;
typedef int     fileFD;
typedef int     ASIO_MODE;
//these two abort on error instead of returning null
inline Var xmalloc(ssize_t size){
    Var x=malloc(size);
    assert(x);
    return x;
};
inline Var xcalloc(ssize_t nmemb,size_t size){
    Var x=calloc(nmemb,size);
    assert(x);
    return x;
};

#ifndef BOOL
#define BOOL unsigned int
#   define TRUE 1
#   define FALSE 0
#endif


typedef struct epoll_event Evt,*h_Evt;
#endif  //!__BASE__H__