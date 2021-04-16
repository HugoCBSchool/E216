#ifndef __HEADER__H__
#define __HEADER__H__

#include "base.h"
#include <sys/types.h>

  
const uint32_t SENTINEL=0x0F0F0F0F;

Class(CommHeader,
    uint32_t    sentinel;
    uint64_t    size;
);

void CommHeaderMarshall(h_CommHeader header);


#endif  //!__HEADER__H__