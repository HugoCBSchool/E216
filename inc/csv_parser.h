#ifndef __CSV_PARSER__H__
#define __CSV_PARSER__H__

#include "base.h"
#include "buffer.h"
#include "csv_handlers.h"
#include "table_string.h"

#define COL_SEP "\t"

Class(CSVColParser,
    CSVCompCtx      ctx;
    str*            fromCSV;
    Var             with;
    ///---------------------------
    BOOL*           ORVAL;
    BOOL*           ANDVAL;
    BOOL            whenNULL;
    //---------------------------
    Var             onArgs;
    f_onBOOL        onTRUE;
    f_onBOOL        onFALSE;
);
Class(CSVParser,
    FILE*           csvFile; 
    h_CSVColParser  colParsers; 
    h_Buffer        buffer; 
    str*            columns;
    BOOL            ORVAL;
    BOOL            ANDVAL;
    int             nbcol;
);
BOOL            CSVColParserRun(h_CSVColParser cp);
h_CSVColParser  CSVParserNew(FILE* file);
sys_t           CSVParserSeekStart(h_CSVParser parser);
ssize_t         CSVParserNext(h_CSVParser parser);
void            CSVParserFree(Var obj);

BOOL            CSVParserRunOR(h_CSVParser parser);
BOOL            CSVParserRunAND(h_CSVParser parser);



#endif  //!__CSV_PARSER__H__