#ifndef __RE__H__
#define __RE__H__

#include <regex.h>
#include <regexp.h>
#include <varargs.h>
#include "base.h"
#include "table_string.h"
#include "client.h"
#include "critere.h"

#include "buffer.h"

#define ANYOF(set)           "[" set "]
#define NONEOF(set)          "[^" set "]"
#define RANGE(start,end)     start "-" end
#define TIMES()              "{" times "}"
#define CAPTURE(pattern)    "(" pattern ")"
#define PARTIAL(expr)       ".*" expr ".*"
#define ANY_YEAR            "\d{1-4}"
#define ANY_INT             "\d+"
#define ANY_FLOAT           "\d+[.]{1}\d+"
#define WORD(pattern)       "\s" pattern "\s"
#define STARTS_WITH(word)   "\s" word
#define ENDS_WITH(word)     word "\s"
#define ANY                 ".*"
#define TAB                 "\t"
#define _EXP(...)   __VA_ARGS__



#define TITLE_RE(   tconst          ,   titleType       ,   primaryTitle,   \
                    originalTitle   ,   isAdult         ,   startYear   ,   \  
                    endYear         ,   runtimeMin      ,   genres      )   \
        tconst          "\t" titleType  "\t" primaryTitle "\t"   \
        originalTitle   "\t" isAdult    "\t" startYear    "\t"   \
        endYear         "\t" runtimeMin "\t" genres            

#define RATE_RE(tconst,averageRating,numVotes)  \
            tconst          \
        TAB averageRating   \
        TAB numVotes        


#define NAME_RE(nconst,primaryName,birthYear,deathYear,primaryProfession,knownForTitles)\
            nconst              TAB primaryName     \
        TAB birthYear           TAB deathYear       \
        TAB primaryProfession   TAB knownForTitles  


str REListToTuple(h_list list, const str sep);
str RECompileList_NoCaptureGroup(h_list list);
str RECompileTable_NoCaptureGroup(h_table_string table);
str RECompileTuple_NoCaptureGroup(str tuple);


Class(RE,
    regex_t     compiled;
    size_t      nbCapture;
    regmatch_t  captured[];
    
);
str      _RECompileTemplate(cstr _template,int nbpatterns,cstr arr[] );
str      RECompileTemplate(cstr _template,int nbpatterns,...);
h_RE     RENewByColumns(int nbColumns,...);
void     REFree(Var obj);
BOOL     REMatch(str buf,h_RE re);
h_list   listTableString_getColumnView(h_list ls,int colid);

#endif  //!__RE__H__