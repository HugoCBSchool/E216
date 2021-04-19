
#include "buffer.h"

h_Buffer    BufferNew(ssize_t size){
    char*    buf_head      =xcalloc(size+1,sizeof(char));
    h_Buffer buf           =xmalloc(sizeof(Buffer));
             buf->buf_head =buf_head;
             buf->buf_tail =&buf_head[size];
             buf->csor_rd  =buf_head;
             buf->csor_wr  =buf_head;
    return buf;
}
extern void BufferFree(Var obj){
    h_Buffer buf=obj;
    
    if(!obj) return;

    free(buf->buf_head);
    memset(buf,0,sizeof(Buffer));
}
Var         BufferMove(h_Buffer buf){
    Var out;
    out=buf->buf_head;
    memset(buf,0,sizeof(Buffer));

    return out;
}
void        BufferInit(h_Buffer buf,ssize_t size){
    if(!buf||!size)
        return;
    if(buf->buf_head)
        free(buf->buf_head);
    buf->buf_head =xcalloc(size+1,sizeof(char));
    buf->buf_tail =&buf->buf_head[size];
    buf->csor_rd  =buf->buf_head;
    buf->csor_wr  =buf->buf_head;
}
ssize_t     BufferIO_recv(h_Buffer buf,ConnFD Connection){
    ssize_t rdcount=0;
    ssize_t avail;
    if(!(avail=BufferGetSizeOKForRead(buf)))
        return 0;//complete
        
    buf->csor_wr+=(
        (rdcount=recv(Connection,buf->csor_rd,avail,0))<0
        ?   0
        :   rdcount
    );
    return (
        rdcount<0
        ?   rdcount
        :   BufferGetSizeOKForRead(buf)
    );
}
ssize_t     BufferIO_read(h_Buffer buf,fileFD file){
    ssize_t rdcount=0;
    ssize_t avail;
    if(!(avail=BufferGetSizeOKForRead(buf)))
        return 0;//complete
        
    buf->csor_wr+=(
        (rdcount=read(file,buf->csor_rd,avail))<0
        ?   0
        :   rdcount
    );
    return (
        rdcount<0
        ?   rdcount
        :   BufferGetSizeOKForRead(buf)
    );
}
ssize_t     BufferIO_send(h_Buffer buf,ConnFD Connection){
    ssize_t wrcount=0;
    ssize_t avail;
    if(!(avail=BufferGetSizeOKForWrite(buf)))
        return 0;//complete
        
    buf->csor_wr+=(
        (wrcount=send(Connection,buf->csor_wr,avail,0))<0
        ?   0
        :   wrcount
    );
    return (
        wrcount<0
        ?   wrcount
        :   BufferGetSizeOKForWrite(buf)
    );
}
ssize_t     BufferIO_write(h_Buffer buf, fileFD file){
    ssize_t wrcount=0;
    ssize_t avail;
    if(!(avail=BufferGetSizeOKForWrite(buf)))
        return 0;//complete
        
    buf->csor_wr+=(
        (wrcount=write(file,buf->csor_wr,avail))<0
        ?   0
        :   wrcount
    );
    return (
        wrcount<0
        ?   wrcount
        :   BufferGetSizeOKForWrite(buf)
    );
}
ssize_t     BufferStream_GetLine(h_Buffer buf,FILE* file){
    size_t sz,nret;
    if(!buf)
        buf=BufferNew(2*sizeof(char));

    sz=BufferGetSizeTotal(buf)+1;
    
    if((nret=getline(&(buf->buf_head),&sz,file))>=0){
        buf->buf_tail=buf->buf_head+sz-1;
        buf->csor_rd =buf->buf_head+nret-1;
        *buf->csor_rd =0;
        buf->csor_wr=buf->buf_head;
    }

    return nret;
}
ssize_t     BufferStream_Write(h_Buffer buf,FILE* file){
    int nret;
    if(!buf->buf_head)
        errno=EINVAL;
        return -1;
    
    if((nret=fputs(buf->csor_wr,file))<0)
        return nret;
    buf->csor_wr+=strlen(buf->csor_wr);

    return BufferGetSizeOKForWrite(buf);
}
ssize_t     BufferGetSizeOKForRead(h_Buffer buf){
    return (
        buf->buf_head
        ?   (buf->buf_tail-buf->csor_rd)
        :   0
    );
}
ssize_t     BufferGetSizeOKForWrite(h_Buffer buf){
    return (
        buf->buf_head
        ?   (buf->buf_tail-buf->csor_wr)+1
        :   0
    );
}
ssize_t     BufferGetSizeTotal(h_Buffer buf){
    if(!buf->buf_head)
        return 0;
    return (buf->buf_tail-buf->buf_head);
}


str  BufferPrintf(h_Buffer buf,const str templ,int nbargs,...){
    va_list     ls;
    str*        args=xcalloc(nbargs,sizeof(str));
    int         len=strlen(templ)-nbargs*strlen("%s");

    {   va_start(ls,nbargs);
        for(int i=0;i<nbargs;++i){
            len+=strlen(
                args[i]=(
                    (args[i]=va_arg(ls,str))
                    ?   args[i]
                    :   ""
                )
            );      
        }
        va_end(ls,nbargs);
    }
    {   BufferInit(buf,(len+1)*sizeof(char));

        str head=templ;
        str tail;
        for(int i=0;i<nbargs;++i){
            tail=strstr(head,"%s");
            strncat(buf->buf_head,head,(tail-head));
            head=tail+strlen("%s");

            strcat(buf->buf_head,args[i]);
        }
        if(*head)
            strcat(buf->buf_head,head);
        
        buf->csor_rd=buf->buf_head+strlen(buf->buf_head);
        buf->csor_wr=buf->buf_head;
    
        free(args);
    }
    
    return buf->buf_head;
}
