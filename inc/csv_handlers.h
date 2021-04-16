#ifndef __CSV_HANDLERS__H__
#define __CSV_HANDLERS__H__

#include "base.h"
#include "buffer.h"
#include "table_string.h"
#include "linklist.h"
#include "hashmap.h"

FtorType(Compare,BOOL,Var fromCSV,Var with);
FtorType(Compare2,BOOL,Var fromCSV,Var with,Var args);
FtorType(onBOOL,void,Var fromCSV,Var with,Var args);
FtorType(Merge,void,Var old,Var New);

Class(CSVCompCtx,
    f_Compare  comp;
    f_Compare2 comp2;
    Var args;
);

static BOOL Cmp_ValueInRange_F(Var fromCSV, Var with){
    float r1=-1.0,r2=-1.0,fl=2.0;
    if(!fromCSV|!with)
        return FALSE;

    if(sscanf((str)fromCSV,"%f",&fl)<=0)
        return FALSE;

    if(sscanf((str)with,"%f:%f",&r1,&r2)<=0)
        return FALSE;
    
    return (fl>=r1&&fl<=r2);
}
static BOOL Cmp_ValueInRange_I(Var fromCSV, Var with){
    int r1=-1,r2=-1,i=2;
    if(!fromCSV|!with)
        return FALSE;

    if(sscanf((str)fromCSV,"%d",&i)<=0)
        return FALSE;

    if(sscanf((str)with,"%d:%d",&r1,&r2)<=0)
        return FALSE;
    
    return (i>=r1&&i<=r2);
}
static BOOL Cmp_RangeHasValue_F(Var fromCSV,Var with){
    return Cmp_ValueInRange_F((str)with,(str)fromCSV);
}
static BOOL Cmp_RangeHasValue_I(Var fromCSV,Var with){
    return Cmp_ValueInRange_I(fromCSV,with);
}
static BOOL Cmp_EQ(Var fromCSV,Var with){
    return(
        fromCSV&&with
        ?(strcmp((str)fromCSV,(str)with)==0)
        :FALSE
    );
}
static BOOL Cmp_NEQ(Var fromCSV,Var with){
    return (
        fromCSV&&with
        ?   !Cmp_EQ((str)fromCSV,(str)with)
        :   FALSE
    );
}

static BOOL Cmp_LHS_LT_F(Var fromCSV,Var with){
    float f1,f2;
    if(!fromCSV|!with)
        return FALSE;
    if(sscanf((str)fromCSV,"%f",&f1)<=0)
        return FALSE;
    if(sscanf((str)with,"%f",&f2)<=0)
        return FALSE;
    return f1<f2;
}
static BOOL Cmp_LHS_GT_F(Var fromCSV,Var with){
    float f1,f2;
    if(!fromCSV|!with)
        return FALSE;
    if(sscanf((str)fromCSV,"%f",&f1)<=0)
        return FALSE;
    if(sscanf((str)with,"%f",&f2)<=0)
        return FALSE;
    return f1>f2;
}
static BOOL Cmp_LHS_LEQ_F(Var fromCSV,Var with){
    float f1,f2;
    if(!fromCSV|!with)
        return FALSE;
    if(sscanf((str)fromCSV,"%f",&f1)<=0)
        return FALSE;
    if(sscanf((str)with,"%f",&f2)<=0)
        return FALSE;
    return f1<=f2;
}
static BOOL Cmp_LHS_GEQ_F(Var fromCSV,Var with){
    float f1,f2;
    if(!fromCSV|!with)
        return FALSE;
    if(sscanf((str)fromCSV,"%f",&f1)<=0)
        return FALSE;
    if(sscanf((str)with,"%f",&f2)<=0)
        return FALSE;
    return f1>=f2;
}

static BOOL Cmp_RHS_LT_F(Var fromCSV,Var with){
    return Cmp_LHS_LT_F((str)with,(str)fromCSV);
}
static BOOL Cmp_RHS_GT_F(Var fromCSV,Var with){
    return Cmp_LHS_GT_F((str)with,(str)fromCSV);
}
static BOOL Cmp_RHS_LEQ_F(Var fromCSV,Var with){
    return Cmp_LHS_LEQ_F((str)with,(str)fromCSV);
}
static BOOL Cmp_RHS_GEQ_F(Var fromCSV,Var with){
    return Cmp_LHS_GEQ_F((str)with,(str)fromCSV);
}

static BOOL Cmp_LHS_LT_I(Var fromCSV,Var with){
    int f1,f2;
    if(!fromCSV|!with)
        return FALSE;
    if(sscanf((str)fromCSV,"%d",&f1)<=0)
        return FALSE;
    if(sscanf((str)with,"%d",&f2)<=0)
        return FALSE;
    return f1<f2;
}
static BOOL Cmp_LHS_GT_I(Var fromCSV,Var with){
    int f1,f2;
    if(!fromCSV|!with)
        return FALSE;
    if(sscanf((str)fromCSV,"%d",&f1)<=0)
        return FALSE;
    if(sscanf((str)with,"%d",&f2)<=0)
        return FALSE;
    return f1>f2;
}
static BOOL Cmp_LHS_LEQ_I(Var fromCSV,Var with){
    int f1,f2;
    if(!fromCSV|!with)
        return FALSE;
    if(sscanf((str)fromCSV,"%d",&f1)<=0)
        return FALSE;
    if(sscanf((str)with,"%d",&f2)<=0)
        return FALSE;
    return f1<=f2;
}
static BOOL Cmp_LHS_GEQ_I(Var fromCSV,Var with){
    int f1,f2;
    if(!fromCSV|!with)
        return FALSE;
    if(sscanf((str)fromCSV,"%d",&f1)<=0)
        return FALSE;
    if(sscanf((str)with,"%d",&f2)<=0)
        return FALSE;
    return f1>=f2;
}

static BOOL Cmp_RHS_LT_I(Var fromCSV,Var with){
    return Cmp_LHS_LT_I((str)with,(str)fromCSV);
}
static BOOL Cmp_RHS_GT_I(Var fromCSV,Var with){
    return Cmp_LHS_GT_I((str)with,(str)fromCSV);
}
static BOOL Cmp_RHS_LEQ_I(Var fromCSV,Var with){
    return Cmp_LHS_LEQ_I((str)with,(str)fromCSV);
}
static BOOL Cmp_RHS_GEQ_I(Var fromCSV,Var with){
    return Cmp_LHS_GEQ_I((str)with,(str)fromCSV);
}

static BOOL Cmp_LHS_Contains(Var fromCSV,Var with){
    if(!fromCSV||!with)
        return FALSE;
    
    return (strstr((str)fromCSV,(str)with)!=NULL);
}
static BOOL Cmp_RHS_Contains(Var fromCSV,Var with){
    if(!fromCSV||!with)
        return FALSE;
    
    return (strstr((str)with,(str)fromCSV)!=NULL);
}

//-----------------------------------------------------------------------
static BOOL Cmp_isInTableString(Var fromCSV,Var with,Var args){
    h_CSVCompCtx ctx=Cast(CSVCompCtx,args);
    
    return table_string_contient((h_table_string)with,ctx->comp,(str)fromCSV);
}
static BOOL Cmp_isInList(Var fromCSV,Var with,Var args){
    BOOL            ret=FALSE;
    h_list          ls =Cast(list,with);
    h_CSVCompCtx    ctx=Cast(CSVCompCtx,args);

    for(str i=list_itor_head(ls);
            i;
            i=list_itor_next(ls)
    ){
        ret|=(
            ctx->comp
            ?   ctx->comp(fromCSV,i)
            :   ctx->comp2(fromCSV,i,ctx->args)
        );
    }
    return ret;
}
static BOOL Cmp_isInHashmapKeys(Var fromCSV,Var with,Var args){
    BOOL        ret=FALSE;
    f_Compare   comp=Cast(CSVCompCtx,args)->comp;
    if(comp==Cmp_EQ)
        return (hmap_get(Cast(hmap,with),(str)fromCSV)!=NULL);
    else
        return FALSE;//not yet done
}
static BOOL Cmp_isInTuple(Var fromCSV,Var with,Var args){
    t_table_string  ts={0};
    BOOL            ret;
    if(!fromCSV||!with)
        return FALSE;
    if(!(table_string_bind(&ts,(str)with,",")))
        return FALSE;

    ret=Cmp_isInTableString(fromCSV,&ts,args);

    if(!(table_string_unbind(&ts,",")))
        return FALSE;
    return ret;
}
static BOOL Cmp_tupleContains(Var fromCSV,Var with,Var args){
    return Cmp_isInTuple(with,fromCSV,args);
}
//-----------------------------------------------------------------------
FtorType(get,Var,Var obj);
Class(CSVHMapCompCtx,
    h_hmap      map;
    f_get       get;
    f_Compare   comp;
    Var         fromCSV;
    BOOL        ORVAL;
);
void CSVHMapCompCtxInit(h_CSVHMapCompCtx ctx,h_hmap map,f_get get){
    ctx->map=map;
    ctx->get=get;
}
static void HmapComp(void* ctx, const char* cle, void* valeur){
    h_CSVHMapCompCtx contx=Cast(CSVHMapCompCtx,ctx);
    contx->ORVAL|=contx->comp(contx->fromCSV,contx->get(valeur));
}
static BOOL Cmp_isInHashmapValues(Var fromCSV,Var with,Var args){
    h_CSVHMapCompCtx ctx=Cast(CSVHMapCompCtx,with);
    ctx->ORVAL  =FALSE;
    ctx->fromCSV=fromCSV;

    hmap_iterer_paires(ctx->map,HmapComp,(Var)ctx);
    return ctx->ORVAL;
}
//-----------------------------------------------------------------------


#endif  //!__CSV_HANDLERS__H__