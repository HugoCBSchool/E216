#include "header.h"

void CommHeaderMarshall(h_CommHeader header){
    if((header->sentinel & SENTINEL))
        return;//already marshalled

    header->size=(uint64_t)(
        ((header->size & 0x00000000000000FF)<<(4*14))| 
        ((header->size & 0x000000000000FF00)<<(4*10))| 
        ((header->size & 0x0000000000FF0000)<<(4*6 ))| 
        ((header->size & 0x00000000FF000000)<<(4*2 ))| 
        ((header->size & 0x00000000000000FF)>>(4*2 ))| 
        ((header->size & 0x000000000000FF00)<<(4*6 ))| 
        ((header->size & 0x0000000000FF0000)>>(4*10))|  
        ((header->size & 0x00000000FF000000)>>(4*14)) 
    );
}