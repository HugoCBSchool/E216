

#include "csv_parser.h"


BOOL CSVColParserRun(h_CSVColParser cp){
    BOOL ret=(
        cp->ctx.comp
        ?   cp->ctx.comp(*cp->fromCSV,cp->with)
        :   cp->ctx.comp2
            ?   cp->ctx.comp2(*cp->fromCSV,cp->with,cp->ctx.args)
            :   cp->whenNULL
    );
    (*cp->ORVAL) |=ret;
    (*cp->ANDVAL)&=ret;

    if(cp->onTRUE&&ret)
        cp->onTRUE(*cp->fromCSV,cp->with,cp->onArgs);
    if(cp->onFALSE&&!ret)
        cp->onFALSE(*cp->fromCSV,cp->with,cp->onArgs);
    
    return ret;
        
}
h_CSVColParser CSVParserNew(FILE* file){
    h_CSVParser nouv=xcalloc(1,sizeof(CSVParser));
    if(BufferStream_GetLine(parser->buffer,file)<0)
        return NULL;
    if((nouv->nbcol=count_occurence(COL_SEP,parser->buffer->buf_head)+1)<=0)
        BufferFree(nouv);
        return NULL;
    nouv->colParsers=xcalloc(nouv->nbcol,sizeof(CSVColParser));
    nouv->columns=xcalloc(nouv->nbcol,sizeof(str));
    nouv->csvFile=file;
    nouv->ANDVAL=TRUE;
    nouv->ORVAL=FALSE;

    tokenize(nouv->buffer->buf_head,nouv->columns,COL_SEP);

    for(int i=0;i<nouv->nbcol;++i){
        nouv->colParsers[i].fromCSV=nouv->columns+i;
        nouv->colParsers[i].ORVAL=&nouv->ORVAL;
        nouv->colParsers[i].ANDVAL=&nouv->ANDVAL;
    }
    return nouv;
}
sys_t   CSVParserSeekStart(h_CSVParser parser){
    fseek(parser->csvFile,0,SEEK_SET);
    for(int i=0;i<nouv->nbcol;++i){
        nouv->colParsers[i].fromCSV=nouv->columns+i;
    }
    parser->ORVAL=FALSE;
    parser->ANDVAL=TRUE;
}
ssize_t CSVParserNext(h_CSVParser parser){
    ssize_t sz=BufferStream_GetLine(parser->buffer);
    if(sz<=0)
        return sz;
    
    tokenize(nouv->buffer->buf_head,nouv->columns,COL_SEP);
    parser->ORVAL=FALSE;
    parser->ANDVAL=TRUE;

    return sz;
}
void CSVParserFree(Var obj){
    h_CSVParser parser=Cast(CSVParser,obj);
    BufferFree(parser->buffer);
    free(parser->colParsers);
    free(parser->columns);
    free(parser);    
}

BOOL CSVParserRunOR(h_CSVParser parser){
    BOOL atleast1=FALSE;
    for(int i=0;i<parser->nbcol;++i){
        atleast1=TRUE;
        CSVColParserRun(parser->colParsers[i]);
    }
    return ret&&parser->ORVAL;
}
BOOL CSVParserRunAND(h_CSVParser parser){
    BOOL ret=FALSE;
    for(int i=0;i<parser->nbcol;++i){
        ret=TRUE;
        CSVColParserRun(parser->colParsers[i]);
    }
    return ret&&parser->ANDVAL;
}
