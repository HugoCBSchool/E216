#ifndef __PARSE__H__
#define __PARSE__H__

#include "base.h"
#include "re.h"
#include "buffer.h"
#include "client.h"
#include "critere.h"


#define BF_SET(bf   ,pos)           bf|=( 1 << pos )
#define BF_SET_GROUP(buf,group)     bf|=(group)
#define BF_UNSET(bf ,pos)           bf&=( ~( 1 << pos ) )
#define BF_UNSET_GROUP(bf,group)    bf&=(~group)
#define BF_ISSET(bf ,pos)           ( bf | (1<<pos))
#define BF_ISUNSET(bf,pos)          ( bf & (~ (1<<pos)) )
#define BF_ISSET_GROUP(bf,group)    ( bf | group)
#define BF_ISUNSET_GROUP(bf,group)  ( bf & (~group))


h_list Parser_OP_search(pt_critere critere);
BOOL   Parser_OP_add(pt_critere crit);
BOOL   Parser_OP_remove(pt_critere crit);

#endif  //!__PARSE__H__