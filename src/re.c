#include "re.h"

str REListToTuple(h_list list, const str sep){
    int len=0;
    int nmemb;
    int nbNonnul=0;
    str membNonnul=NULL;

    if(!list||!(nmemb=list_get_nb_elem(list)))
        return NULL;

    for(str i=(str)list_itor_head(list);
        nmemb;
        i=(str)list_itor_next(list)
    ){
        if(!i||!strlen(i)){
            --nmemb;
            continue;
        }
        len+=strlen(i);
        --nmemb;
        membNonnul=i;
        ++nbNonnul;
    }
    if(!nbNonnul)
        return NULL;
    if(nbNonnul==1)
        return strdup(membNonnul);

    len+=(nbNonnul-1)*strlen(sep);

    membNonnul=(str)xcalloc(len+1,sizeof(char));



    for(str i=(str)list_itor_head(list);
        nbNonnul;
        i=(str)list_itor_next(list)
    ){
        if(!i||!strlen(i)){
            continue;
        }
        strcat(membNonnul,i);
        strcat(membNonnul,sep);
        --nbNonnul;
    }

    return membNonnul;
}
str RECompileList_NoCaptureGroup(h_list list){
    int len=0;
    int nmemb;
    int nbNonnul=0;
    str membNonnul=NULL;

    if(!list||!(nmemb=list_get_nb_elem(list)))
        return NULL;

    for(str i=(str)list_itor_head(list);
        nmemb;
        i=(str)list_itor_next(list)
    ){
        if(!i||!strlen(i)){
            --nmemb;
            continue;
        }
        len+=strlen(i);
        --nmemb;
        membNonnul=i;
        ++nbNonnul;
    }
    if(!nbNonnul)
        return NULL;
    if(nbNonnul==1)
        return strdup(membNonnul);

    len+=strlen("(?:)")+nbNonnul;

    membNonnul=(str)xcalloc(len,sizeof(char));

    strcat(membNonnul,"(?:");

    for(str i=(str)list_itor_head(list);
        nbNonnul;
        i=(str)list_itor_next(list)
    ){
        if(!i||!strlen(i)){
            continue;
        }
        strcat(membNonnul,i);
        strcat(membNonnul,"|");
        --nbNonnul;
    }
    membNonnul[strlen(membNonnul)-1]=')';

    return membNonnul;
}
str RECompileTable_NoCaptureGroup(h_table_string table){
    int sz=0;
    int nonnul=0;
    int nnindex;
    for(int i=0;i<table->nb_element;++i){
        if(!table->table[i]||!strlen(table->table[i]))
            continue;
        ++nonnul;
        nnindex=i;
        sz+=strlen(table->table[i])+strlen("|");
    }
    switch(nonnul){
        case 0:return NULL;
        case 1:return strdup(table->table[nnindex]);
        default:break;
    }
    sz+=strlen("(?:)");

    char* ret=(char*)xcalloc(sz,sizeof(char));
    strcat(ret,"(?:");
    while(nonnul){
        if(!table->table[nonnul]||!strlen(table->table[nonnul]))
            continue;
        strcat(ret,table->table[nonnul]);
        strcat(ret,"|");
        --nonnul;
    }
    ret[strlen(ret)-1]=')';

    return ret;
}
str RECompileTuple_NoCaptureGroup(str tuple){
    int count=count_occurence(",",tuple)+1;
    str arr[]=(str*)xcalloc(count,sizeof(str));
    tokenize(tuple,arr,",");
    
    int sz=0;
    int nonnul=0;
    int nnindex;
    for(int i=0;i<count;++i){
        if(!arr[i]||!strlen(arr[i]))
            continue;
        ++nonnul;
        nnindex=i;
        sz+=strlen(arr[i])+strlen("|");
    }
    switch(nonnul){
        case 0:return NULL;
        case 1:return strdup(arr[nnindex]);
        default:break;
    }
    sz+=strlen("(?:)");

    char* ret=(char*)xcalloc(sz,sizeof(char));
    strcat(ret,"(?:");
    while(nonnul){
        if(!arr[nonnul]||!strlen(arr[nonnul]))
            continue;
        strcat(ret,arr[nonnul]);
        strcat(ret,"|");
        --nonnul;
    }
    untokenize(arr,count,",");
    free(arr);
    ret[strlen(ret)-1]=')';

    return ret;
}


Class(RE,
    regex_t     compiled;
    size_t      nbCapture;
    regmatch_t* captured;
    
);
str      _RECompileTemplate(cstr _template,int nbpatterns,cstr arr[] ){
    int sz=0;
    str ret=NULL;
    for(int i=0;i<nbpatterns;++i)
        sz+=strlen(arr[i])-strlen("%s");
    
    ret=(str)xcalloc(sz+1,sizeof(char));
    
    int i=0;
    str head=_template;
    for(int i=0;i<nbpatterns;++i){
        str tail=strstr(head,"%s");
        strncat(ret,head,(tail-head));
        strcat(ret,arr[i]);
        head=tail+strlen("%s");
    }
    if(*head)
        strcat(ret,head);
    

    return ret;
}
str  RECompileTemplate(cstr _template,int nbpatterns,...){
    va_list ls;
    str ret;
    str* arr=(str*)xcalloc(nbpatterns,sizeof(str));

    va_start(ls,nbpatterns);
    for(int i=0;i<nbpatterns;++i){
        arr[i]=va_arg(ls,str);
    }
    ret=_RECompileTemplate(_template,nbpatterns,arr);
    free(arr);
    va_end(ls);
    return ret;
}
h_RE RENewByColumns(int nbColumns,...){
    int     sz              =0;
    h_RE    nouv            =xcalloc(1,sizeof(RE));
    str     tmpstr          =NULL;
    str     templateArg[]   =xcalloc(nbColumns+1,sizeof(str));
    str     _template       =xcalloc(nbColumns*strlen("%s\t"),sizeof(char));
    int     tablen          =strlen(TAB);
    cstr    anymatch        =ANY;
    str     regexpr         =NULL;
    int     nbNonnul        =0;

    for(int i=0;i<nbColumns;++i)
        strcat(_template,"%s\t");
    *(_template+strlen(_template)-1)='\0';


    {   //unpack
        va_list ls;

        va_start(ls,nbColumns);
        for(int i=0;i<nbColumns;++i){
            templateArg[i]=(
                ((tmpstr=va_arg(ls,str))==NULL)
                ?   anymatch
                :   (*tmpstr=='\0')
                    ?   anymatch
                    :   tmpstr            
            );
            nbNonnul+=(templateArg[i]==anymatch);
        }
        va_end(ls);
    }
    if(!nbNonnul){
        free(nouv);
        free(_template);
        free(templateArg);
        return NULL;
    }

    if(!(regexpr=_RECompileTemplate(_template,nbColumns,templateArg))){
        free(nouv);
        free(_template);
        free(templateArg);
        return NULL;
    }
    free(_template);
    free(templateArg);

    if(regcomp(&nouv->compiled,regexpr,REG_EXTENDED|REG_ICASE|REG_NEWLINE)!=0){
        free(nouv);
        return NULL;
    }

    nouv->captured=(regmatch_t*)xcalloc(nbNonnul+1,sizeof(regmatch_t));
    nouv->nbCapture=nbNonnul;

    return nouv;
}
void REFree(Var obj){
    h_RE re=Cast(RE,obj);
    free(re->captured);
    regfree(&re->compiled);
    free(re);
}

BOOL REMatch(str buf,h_RE re){
    return (
        regexec(
            &re->compiled,buf,re->nbCapture,re->captured,0
        )==0
    );
}

h_list listTableString_getColumnView(h_list ls,int colid){
    h_list ls2=list_new(NULL,NULL);
    for(    h_table_string i=(h_table_string)list_itor_head(ls);
            i;
            i=(h_table_string)list_itor_next(ls)
    ){
        if(i->nb_element<=colid)
            continue;
        list_push_head(ls2,i->table[colid]);
    }
    return ls2;
}

