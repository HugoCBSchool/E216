
#ifndef __BUFFER__H__
#define __BUFFER__H__

#include "base.h"
#define PGSIZE 4096

Class(Buffer,
    char* buf_head;
    char* buf_tail;
    char* csor_rd;
    char* csor_wr;
);
//---------------------------------------------------------------
h_Buffer    BufferNew(ssize_t size);
extern void BufferFree(Var obj);
Var         BufferMove(h_Buffer buf);
void        BufferInit(h_Buffer buf,ssize_t size);
//---------------------------------------------------------------
//recv but returns the remaining expected number of bytes before buffer is full
ssize_t BufferIO_recv(h_Buffer buf,ConnFD Connection);
ssize_t BufferIO_read(h_Buffer buf,fileFD file);
//send/ but returns the remaining size of the buffer before empty
ssize_t BufferIO_send(h_Buffer buf,ConnFD Connection);
ssize_t BufferIO_write(h_Buffer buf, fileFD file);
//---------------------------------------------------------------
// get the next line from the file descriptor
//resizing is automatic
//the old content is overwritten. 
//use BufferMove to take out the underlying string if you dont want to overwrite
//the \n is removed
ssize_t  BufferStream_GetLine(h_Buffer buf,FILE* file);
//writes to the file the content of the buffer and adds a \n to it
//use BufferMove to take out the underlying string if you dont want to overwrite
//the \n is removed
ssize_t  BufferStream_Write(h_Buffer buf,FILE* file);
//informatrices
ssize_t  BufferGetSizeOKForRead(h_Buffer buf);
ssize_t  BufferGetSizeOKForWrite(h_Buffer buf);
ssize_t  BufferGetSizeTotal(h_Buffer buf);

#endif  //!__BUFFER__H__