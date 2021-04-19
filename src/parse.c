#include "parse.h"


#define DB_PATH "IMDB_PATH"
#define NBCOL_TITLE 9
#define NBCOL_NAME 6
#define NBCOL_RATING 3
#define MAXTAGLEN 256

#define DB_TITLE "title_basics.tsv"
#define DB_RATING "title_ratings.tsv"
#define DB_NAME   "name_basics.tsv"
#define DB_TMP(x) x "tmp"

static fileFD dbroot=-1;
static h_list list_name_tags=NULL;
static pthread_mutex_t list_name_mutx=PTHREAD_MUTEX_INITIALIZER;
static h_list list_title_tags=NULL;
static pthread_mutex_t list_title_mutx=PTHREAD_MUTEX_INITIALIZER;

inline long int getTagValue(const str tag){ 
    return atoll(tag+2);
}
static BOOL tagLEQ(Var lhs,Var rhs){
    return(getTagValue((str)lhs)<=getTagValue((str)rhs));
}
inline void addTagToTitlelist(str tag){
    long int lhs=getTagValue(tag);
    pthread_mutex_lock(&list_title_mutx);
    for(str i=(str)list_itor_head(list_title_tags);
        i;
        i=(str)list_itor_next(list_title_tags)
    ){
        if(lhs==getTagValue(i))
            free(tag);
            goto _skip;
    }
    list_push_sort(list_title_tags,tag);
    _skip:
    pthread_mutex_unlock(&list_title_mutx);
}
inline void addTagToNamelist(str tag){
    long int lhs=getTagValue(tag);
    pthread_mutex_lock(&list_name_mutx);
    for(str i=(str)list_itor_head(list_name_tags);
        i;
        i=(str)list_itor_next(list_name_tags)
    ){
        if(lhs==getTagValue(i))
            free(tag);
            goto _skip;
    }
    list_push_sort(list_name_tags,tag);
    _skip:
    pthread_mutex_unlock(&list_name_mutx);
}
void updateTitleTag(str* lastTag,str line){
    str csor=NULL;
    char tag[MAXTAGLEN+1]={0};
    char tag2[MAXTAGLEN+1]={0};
    if((csor=strchr(line,'\t'))){
        *csor='\0';
    }
    if(!*lastTag){
        *lastTag=strdup(line);
    }else{
        if(atoll(*lastTag+2)!=atoll(line+2)-1){
            for(long int i=getTagValue(*lastTag);
                i<getTagValue(line);
                ++i
            ){
                memset(tag,'0',strlen(line)*sizeof(char));
                strncpy(tag,line,2);
                memset(tag2,0,MAXTAGLEN*sizeof(char));
                sprintf(tag2,"%lld",i);
                strcpy(tag+strlen(tag)-strlen(tag2),tag2);
                addTagToTitlelist(strdup(tag));
            }
            free(*lastTag);
            *lastTag=strdup(line);

        }else{
            free(*lastTag);
            *lastTag=strdup(line);
        }
    }
    if(csor)
        *csor='\t';
}
void updateNameTag(str* lastTag,str line){
    str csor=NULL;
    char tag[MAXTAGLEN+1]={0};
    char tag2[MAXTAGLEN+1]={0};
    if((csor=strchr(line,'\t'))){
        *csor='\0';
    }
    if(!*lastTag){
        *lastTag=strdup(line);
    }else{
        if(atoll(*lastTag+2)!=atoll(line+2)-1){
            for(long int i=getTagValue(*lastTag);
                i<getTagValue(line);
                ++i
            ){
                memset(tag,'0',strlen(line)*sizeof(char));
                strncpy(tag,line,2);
                memset(tag2,0,MAXTAGLEN*sizeof(char));
                sprintf(tag2,"%lld",i);
                strcpy(tag+strlen(tag)-strlen(tag2),tag2);
                addTagToNamelist(strdup(tag));
            }
            free(*lastTag);
            *lastTag=strdup(line);

        }else{
            free(*lastTag);
            *lastTag=strdup(line);
        }
    }
    if(csor)
        *csor='\t';
}
void dbInit(){
    pthread_mutex_unlock(&list_name_mutx);
    if(dbroot>0){
        pthread_mutex_unlock(&list_name_mutx);
        return ;
    }
    {//init db root
        char* dbpath=getenv(DB_PATH);
        assert((dbroot=open(DB_PATH,O_DIRECTORY))!=-1);
        free(dbpath);
    }
    {//init taglists
        h_Buffer    buf      =BufferNew(2);
        fileFD      f        =openat(dbroot,DB_NAME,O_RDONLY);
        FILE*       db       =fdopen(f,"rt");
        str         lastTag  =NULL;

        assert(BufferStream_GetLine(buf,db)>0);
        assert((list_name_tags=list_new(free,tagLEQ))!=NULL);

        while(BufferStream_GetLine(buf,db)>0)
            updateNameTag(&lastTag,buf->buf_head);
        
        BufferFree(buf);
        fclose(db);
        close(f);

        if(!list_get_nb_elem(list_name_tags)){
            long int i=getTagValue(lastTag)+1;
            int sz=2;
            char tag[MAXTAGLEN+1]={0};
            char tag2[MAXTAGLEN+1]={0};
            BOOL incr=TRUE;
            for(str csor=&lastTag[2];*csor;++csor){
                incr&=(*csor=='9');
            }
            memset(tag,'0',(strlen(lastTag)+incr)*sizeof(char));
            strncpy(tag,lastTag,2);
            sprintf(tag2,"%lld",i);
            strcpy(tag+strlen(tag)-strlen(tag2),tag2);
            addTagToNamelist(strdup(tag));
        }
        free(lastTag);
    }
    {//init taglists
        h_Buffer    buf      =BufferNew(2);
        fileFD      f        =openat(dbroot,DB_TITLE,O_RDONLY);
        FILE*       db       =fdopen(f,"rt");
        str         lastTag  =NULL;

        assert(BufferStream_GetLine(buf,db)>0);
        assert((list_title_tags=list_new(free,tagLEQ))!=NULL);

        while(BufferStream_GetLine(buf,db)>0)
            updateTitleTag(&lastTag,buf->buf_head);
        
        BufferFree(buf);
        fclose(db);
        close(f);

        if(!list_get_nb_elem(list_title_tags)){
            long int i=getTagValue(lastTag)+1;
            int sz=2;
            char tag[MAXTAGLEN+1]={0};
            char tag2[MAXTAGLEN+1]={0};
            BOOL incr=TRUE;
            for(str csor=&lastTag[2];*csor;++csor){
                incr&=(*csor=='9');
            }
            memset(tag,'0',(strlen(lastTag)+incr)*sizeof(char));
            strncpy(tag,lastTag,2);
            sprintf(tag2,"%lld",i);
            strcpy(tag+strlen(tag)-strlen(tag2),tag2);
            addTagToTitlelist(strdup(tag));
        }
        free(lastTag);
    }
    pthread_mutex_unlock(&list_name_mutx);
}
str listTagPopTag(h_list list){
    str atag=(str)list_pop_head(list);
    
    if(list_get_nb_elem(list))
        return atag;


    long int i=getTagValue(atag)+1;
    int sz=2;
    char tag[MAXTAGLEN+1]={0};
    char tag2[MAXTAGLEN+1]={0};
    BOOL incr=TRUE;
    for(str csor=&atag[2];*csor;++csor){
        incr&=(*csor=='9');
    }
    memset(tag,'0',(strlen(atag)+incr)*sizeof(char));
    strncpy(tag,atag,2);
    sprintf(tag2,"%lld",i);
    strcpy(tag+strlen(tag)-strlen(tag2),tag2);
    list_push_head(list,strdup(tag));
    return atag;
}
h_RE REBuild_TITLE_SEARCH(pt_critere critere){
    str     re_title=NULL;
    h_RE    re      =NULL;


    if(!critere->titre||!(critere->titre->nb_element))
        return NULL;
    
    {//build partials
        str re_tuple=RECompileTable_NoCaptureGroup(critere->titre);
        re_title=(
            re_tuple
            ?   RECompileTemplate(CAPTURE(PARTIAL("%s")),1,re_tuple)
            :   NULL
        );
        free(re_tuple);
        if(!re_title)
            return NULL;
    }

    {//build
        re=RENewByColumns(NBCOL_TITLE,
            CAPTURE(ANY),
            ANY,
            ANY,
            re_title,
            ANY,
            CAPTURE(ANY_YEAR),
            ANY,
            ANY,
            ANY
        );
        free(re_title);
    }

    return re;
}
h_RE REBuild_NAME_SEARCH(pt_critere critere, h_list title_match){
    h_list title_tags=listTableString_getColumnView(title_match,0);
    str    re_tt;
    str    re_name;
    h_RE   re;

    if(!title_tags||!list_get_nb_elem(title_tags))
        list_free(title_tags);


    {//build partials
        str reGr_tt=RECompileList_NoCaptureGroup(title_tags);
        re_tt=(
            reGr_tt
            ?   RECompileTemplate(CAPTURE(PARTIAL("%s")),1,reGr_tt)
            :   NULL
        );
        if(!re_tt){
            free(reGr_tt);
            list_free(title_tags);
            return NULL;
        }
        free(reGr_tt);
        list_free(title_tags);
    }
    {//build partials
        str reGr_name=RECompileTable_NoCaptureGroup(critere->personne);
        re_name=(
            reGr_name
            ?   RECompileTemplate(CAPTURE(PARTIAL("%s")),1,reGr_name)
            :   strdup(CAPTURE(ANY))
        );
        free(reGr_name);
        if(!re_name)
            return NULL;
    }
    {//build
        re=RENewByColumns(NBCOL_NAME,
            CAPTURE(ANY),
            re_name,
            ANY,
            ANY,
            ANY,
            re_tt
        );
        free(re_name);
        free(re_tt);
    }

    return re;
}
h_RE REBuild_RATING_SEARCH(pt_critere critere,h_list title_match){
    h_list  title_tags=listTableString_getColumnView(title_match,0);
    str     re_tt;
    h_RE    re;

    if(!title_tags||!list_get_nb_elem(title_tags))
        list_free(title_tags);

    
    {//build partials
        str reGr_tt=RECompileList_NoCaptureGroup(title_tags);
        re_tt=(
            reGr_tt
            ?   RECompileTemplate(CAPTURE(PARTIAL("%s")),1,reGr_tt)
            :   NULL
        );
        if(!re_tt){
            free(reGr_tt);
            list_free(title_tags);
            return NULL;
        }
        free(reGr_tt);
        list_free(title_tags);
    }
    {//build
        re=RENewByColumns(NBCOL_NAME,
            re_tt,
            CAPTURE(ANY_FLOAT),
            ANY
        );
        free(re_tt);
    }
    return re;
}

h_list Parser_OP_search(pt_critere critere){
    h_list      ls_title=list_new(table_string_liberer,NULL);
    h_list      ls_rating=list_new(table_string_liberer,NULL);
    h_list      ls_name=list_new(table_string_liberer,NULL);
    dbInit();

    {//title search
        h_Buffer    buf      =BufferNew(2);
        h_RE        re_title =REBuild_TITLE_SEARCH(critere);
        fileFD      f        =openat(dbroot,DB_TITLE,O_RDONLY);
        FILE*       db       =fdopen(f,"rt");

        assert(BufferStream_GetLine(buf,db)>0);
        while(BufferStream_GetLine(buf,db)){
            if(REMatch(buf->buf_head,re_title)){
                h_table_string rtable=table_string_bind2(
                    (str)BufferMove(buf),
                    "\t"
                );
                _match_year:{
                    str         ryear  =rtable->table[5];
                    if(critere->annee&&critere->annee->nb_element){
                        int   i1=-1  ,i2=-1,i3=atoi(ryear);
                        sscanf(critere->annee->table[0],"%d:%d",&i1,&i2);
                        sscanf(ryear,"%d",&i3);
                        if(!(
                            i3
                            ?   (i1>0)
                                ?   (i2>0)
                                    ?   (i3>=i1&&i3<=i2)
                                    :   (i3>=i1)
                                :   (i2>0)
                                    ?   (i3>=i2)
                                    :   TRUE
                            :   FALSE
                        )){
                            table_string_liberer(rtable);
                            goto _again1;
                        }
                    }
                }
                list_push_tail(ls_title,rtable);
            }
            _again1:
        }   
        BufferFree(buf);
        REFree(re_title);
        fclose(db);
        close(f);
    }
    {//rating search
        h_Buffer    buf      =BufferNew(2);
        h_RE        re_rating=REBuild_RATING_SEARCH(critere,ls_title);
        fileFD      f        =openat(dbroot,DB_RATING,O_RDONLY);
        FILE*       db       =fdopen(f,"rt");
        assert(BufferStream_GetLine(buf,db)>0);

        while(BufferStream_GetLine(buf,db)){
            if(REMatch(buf->buf_head,re_rating)){
                h_table_string rtable=table_string_bind2(
                    (str)BufferMove(buf),
                    "\t"
                );
                _match_rating:{
                    str         rrating  =rtable->table[1];
                    if(critere->cote&&critere->cote->nb_element){
                        float f1=-1.0,f2=-1.0,f3=-1;
                        int   i1=-1  ,i2=-1;
                        sscanf(critere->cote->table[0],"%d:%d",&i1,&i2);
                        sscanf(critere->cote->table[0],"%lf:%lf",&f1,&f2);
                        sscanf(rrating,"%lf",&f3);
                        f1=((f1>=0.0)?f1:(i1>=0)?(i1*1.0):-1.0);
                        f2=((f2>=0.0)?f2:(i2>=0)?(i2*1.0):-1.0);
                        f3=((f3>=0.0)
                            ?   (f1>=0.0)
                                ?   (f2>=0.0)
                                    ?   (f3>=f1&&f3<=f2)? f3 : -1.0
                                    :   (f3>=f1)        ? f3 : -1.0
                                :   (f2>=0.0)
                                    ?   (f3>=f2)? f3 : -1.0
                                    :   f3
                            :   f3
                        );
                        if(!(f3>=0.0)){
                            table_string_liberer(rtable);
                            goto _again2;
                        }
                    }
                }
                _match_tag1:{//match the 
                    str         rtag =rtable->table[0];
                    h_table_string last=(h_table_string)list_pop_head(ls_title);
                    list_push_tail(ls_title,last);
                    BOOL again=TRUE;
                    for(h_table_string cur=(h_table_string)list_pop_head(ls_title);
                        again;
                        cur=(h_table_string)list_pop_head(ls_title)
                    ){
                        again=(cur==last);
                        if(strcmp(cur->table[0],rtag)!=0){
                            list_push_tail(ls_title,cur);
                            continue;
                        }
                        list_push_tail(ls_rating,cur);
                    }
                    table_string_liberer(rtable);
                }
            }
            _again2:
        }
        
        table_string_liberer(ls_title);
        BufferFree(buf);
        close(f);
        fclose(db);
        REFree(re_rating);
    }
    {//name search
        h_Buffer    buf      =BufferNew(2);
        h_RE        re       =REBuild_NAME_SEARCH(critere,ls_rating);
        fileFD      f        =openat(dbroot,DB_NAME,O_RDONLY);
        FILE*       db       =fdopen(f,"rt");

        assert(BufferStream_GetLine(buf,db)>0);

        while(BufferStream_GetLine(buf,db)){
            if(REMatch(buf->buf_head,re)){
                h_table_string rtable=table_string_bind2(
                    (str)BufferMove(buf),
                    "\t"
                );
                _match_tag2:{//match the 
                    str         rtag =rtable->table[0];
                    h_table_string last=(h_table_string)list_pop_head(ls_rating);
                    list_push_tail(ls_title,last);
                    BOOL again=TRUE;
                    for(h_table_string cur=(h_table_string)list_pop_head(ls_rating);
                        again;
                        cur=(h_table_string)list_pop_head(ls_title)
                    ){
                        again=(cur==last);
                        if(strcmp(cur->table[0],rtag)!=0){
                            list_push_tail(ls_name,cur);
                            continue;
                        }
                        list_push_tail(ls_name,cur);
                    }
                    table_string_liberer(rtable);
                }
            }
            _again3:
        }
        
        table_string_liberer(ls_rating);
        BufferFree(buf);
        close(f);
        fclose(db);
        REFree(re);
    }
    return ls_name;
}
BOOL   Parser_OP_add(pt_critere crit){
    h_list          ls_name;
    str             titletag=NULL;
    str             nametag=NULL;
    BOOL            title_modified=FALSE;
    BOOL            title_appended=FALSE;

    dbInit();


    {//title
        fileFD  fi      =openat(dbroot,DB_TITLE,O_RDONLY);
        fileFD  fo      =openat(dbroot,DB_TMP(DB_TITLE),O_WRONLY|O_TRUNC|O_CREAT);
        FILE*   dbi     =fdopen(fi,"wt");
        FILE*   dbo     =fdopen(fo,"wt");
        h_RE    re_title=RENewByColumns(NBCOL_TITLE,
                            ANY,ANY,ANY,
                            crit->titre->table[0],
                            ANY,ANY,ANY,ANY,ANY
                        );


        h_Buffer    buf      = BufferNew(2);
        int         has_field=((1<<9)-1);
        str         tmp=NULL;
        {   //prepare the insertion template
            has_field=((1<<9)-1);
            BF_UNSET(has_field,0);
            if(!crit->type_titre)
                BF_UNSET(has_field,1);
            if(!crit->annee->table[0])
                BF_UNSET(has_field,5);
            if(!crit->annee->table[0]||!(tmp=strstr(crit->annee->table[0],":")))
                BF_UNSET(has_field,6);
            BF_UNSET(has_field,7);
            BF_UNSET(has_field,8);

        }

        if(tmp) *tmp='\0';

        BufferStream_GetLine(buf,dbi);
        BufferStream_Write(buf,dbo);

        BOOL done=FALSE;
        while(BufferStream_GetLine(buf,dbi)>0){
            if(REMatch(buf->buf_head,re_title)&&!done){
                h_table_string old=table_string_bind2((str)BufferMove(buf),"\t");
                BufferPrintf(buf,TITLE_RE("%s","%s","%s","%s","%s","%s","%s","%s","%s"),9,
                    old->table[0],
                    ( BF_ISSET(has_field,1) ? crit->type_titre      : old->table[1]),
                    crit->titre->table[0],
                    crit->titre->table[0],
                    old->table[4],
                    ( BF_ISSET(has_field,5) ? crit->annee->table[0] : old->table[5]),
                    ( BF_ISSET(has_field,6) ? tmp                   : old->table[6]),
                    old->table[7],
                    old->table[8]
                );
                table_string_liberer(old);
                done=TRUE;
                title_modified=TRUE;
            }
            BufferStream_Write(buf,dbo);
        }
        if(!done){
            title_appended=TRUE;
            titletag=listTagPopTag(list_title_tags);
            BufferPrintf(buf,TITLE_RE("%s","%s","%s","%s","%s","%s","%s","%s","%s"),9,
                titletag,
                ( BF_ISSET(has_field,1) ? crit->type_titre      : "short"),
                crit->titre->table[0],
                crit->titre->table[0],
                "0",
                ( BF_ISSET(has_field,5) ? crit->annee->table[0] : "\\N"),
                ( BF_ISSET(has_field,6) ? tmp                   : "\\N"),
                "\\N",
                "\\N"
            );
            BufferStream_Write(buf,dbo);
        }
        BufferFree(buf);
        fclose(dbi);fclose(dbo);
        close(fi);close(fo);
        renameat2(dbroot,DB_TITLE,dbroot,DB_TMP(DB_TITLE),RENAME_EXCHANGE);
        REFree(re_title);
    }
    do{//rating
        if(title_appended){
            fileFD  fi      =openat(dbroot,DB_RATING,O_WRONLY);
            FILE*   dbi     =fdopen(fi,"a");
            h_Buffer buf=BufferNew(2);
            BufferPrintf(buf,RATE_RE("%s","%s","%s"),3,
                titletag,
                (   (crit->cote&&crit->cote->nb_element)
                    ?   crit->cote->table[0]
                    :   "10.0"
                ),
                "1"
            );
            BufferStream_Write(buf,dbi);
            BufferFree(buf);
            fclose(dbi);
            close(fi);
            break;
        }

        if(!(crit->cote)||!crit->cote->nb_element)
            break;

        fileFD  fi      =openat(dbroot,DB_RATING,O_RDONLY);
        fileFD  fo      =openat(dbroot,DB_TMP(DB_RATING),O_WRONLY|O_TRUNC|O_CREAT);
        FILE*   dbi     =fdopen(fi,"wt");
        FILE*   dbo     =fdopen(fo,"wt");
        h_RE    re_rating=RENewByColumns(NBCOL_RATING,
                            titletag,
                            ANY,
                            ANY
                        );
        h_Buffer    buf      = BufferNew(2);

        BufferStream_GetLine(buf,dbi);
        BufferStream_Write(buf,dbo);

        BOOL done=FALSE;
        while(BufferStream_GetLine(buf,dbi)>0){
            if(REMatch(buf->buf_head,re_rating)&&!done){
                h_table_string old=table_string_bind2((str)BufferMove(buf),"\t");
                int nbvotes;
                char currating_s[5]={0};
                char nbvotes_s[256]={0};

                if(!(nbvotes=atoi(old->table[2])+1))
                    strcpy(nbvotes_s,"1");
                else
                    snprintf(nbvotes_s,255*sizeof(char),"%d",nbvotes);

                sprintf(currating_s,"%lf2.1",
                    (atof(old->table[1])*nbvotes+atof(crit->cote->table[0]))/nbvotes);
                BufferPrintf(buf,RATE_RE("%s","%s","%s"),3,
                    old->table[0],
                    currating_s,
                    nbvotes_s
                );
                table_string_liberer(old);
                done=TRUE;
            }
            BufferStream_Write(buf,dbo);
        }
        BufferFree(buf);
        fclose(dbi);fclose(dbo);
        close(fi);close(fo);
        renameat2(dbroot,DB_RATING,dbroot,DB_TMP(DB_RATING),RENAME_EXCHANGE);
        REFree(re_rating);
    }while(0);
    do{//name
        if(!crit->personne||!crit->personne->nb_element)
            break;

        fileFD      fi      =openat(dbroot,DB_RATING,O_RDONLY);
        fileFD      fo      =openat(dbroot,DB_TMP(DB_RATING),O_WRONLY|O_TRUNC|O_CREAT);
        FILE*       dbi     =fdopen(fi,"wt");
        FILE*       dbo     =fdopen(fo,"wt");
        h_RE        re_name =RENewByColumns(NBCOL_NAME,
                                ANY,
                                crit->personne->table[0],
                                ANY,
                                ANY,
                                ANY,
                                ANY
                            );
        h_Buffer    buf     =BufferNew(2);

        BufferStream_GetLine(buf,dbi);
        BufferStream_Write(buf,dbo);

        BOOL done=FALSE;
        while(BufferStream_GetLine(buf,dbi)>0){
            if(REMatch(buf->buf_head,re_name)&&!done){
                str             old     =(str)BufferMove(buf);
                h_table_string  old_t   =table_string_bind2(old,"\t");
                BOOL            first   =!(strlen(old_t->table[5]));
                const char*       templ =(first?"%s%s":"%s,%s");

                table_string_unbind(old_t,"\t");
                BufferPrintf(buf,(str)templ,2,old,titletag);
                table_string_liberer(old_t);
                free(old);
                done=TRUE;
            }
            BufferStream_Write(buf,dbo);
        }

        BufferFree(buf);
        fclose(dbi);fclose(dbo);
        close(fi);close(fo);
        renameat2(dbroot,DB_NAME,dbroot,DB_TMP(DB_NAME),RENAME_EXCHANGE);
        REFree(re_name);

    }while(0);
    
    return TRUE;

}
BOOL   Parser_OP_remove(pt_critere crit){
    str titletag;
    dbInit();

    {//title
        fileFD  fi      =openat(dbroot,DB_TITLE,O_RDONLY);
        fileFD  fo      =openat(dbroot,DB_TMP(DB_TITLE),O_WRONLY|O_TRUNC|O_CREAT);
        FILE*   dbi     =fdopen(fi,"wt");
        FILE*   dbo     =fdopen(fo,"wt");
        h_RE    re_title=RENewByColumns(NBCOL_TITLE,
                            ANY,ANY,ANY,
                            crit->titre->table[0],
                            ANY,ANY,ANY,ANY,ANY
                        );

        h_Buffer    buf      = BufferNew(2);
        int         has_field=((1<<9)-1);
        str         tmp=NULL;

        if(tmp) *tmp='\0';

        BufferStream_GetLine(buf,dbi);
        BufferStream_Write(buf,dbo);

        BOOL done=FALSE;
        while(BufferStream_GetLine(buf,dbi)>0){
            if(REMatch(buf->buf_head,re_title)&&!done){

                h_table_string old=table_string_bind2((str)BufferMove(buf),"\t");
                titletag=strdup(old->table[0]);
                addTagToTitlelist(titletag);
                table_string_liberer(old);
                done=TRUE;
            }else{
                BufferStream_Write(buf,dbo);
            }
        }
            
        BufferFree(buf);
        fclose(dbi);fclose(dbo);
        close(fi);close(fo);
        renameat2(dbroot,DB_TITLE,dbroot,DB_TMP(DB_TITLE),RENAME_EXCHANGE);
        REFree(re_title);
        if(!done)
            return FALSE;
    }
    {//rating search
        fileFD  fi      =openat(dbroot,DB_RATING,O_RDONLY);
        fileFD  fo      =openat(dbroot,DB_TMP(DB_RATING),O_WRONLY|O_TRUNC|O_CREAT);
        FILE*   dbi     =fdopen(fi,"wt");
        FILE*   dbo     =fdopen(fo,"wt");
        h_RE    re_rate =RENewByColumns(NBCOL_RATING,
                            titletag,
                            ANY,
                            ANY
                        );
        h_Buffer    buf =BufferNew(2);

        BufferStream_GetLine(buf,dbi);
        BufferStream_Write(buf,dbo);

        BOOL done=FALSE;
        while(BufferStream_GetLine(buf,dbi)>0){
            if(REMatch(buf->buf_head,re_rate)&&!done){
                done=TRUE;
            }else{
                BufferStream_Write(buf,dbo);
            }
        }
        BufferFree(buf);
        fclose(dbi);fclose(dbo);
        close(fi);close(fo);
        renameat2(dbroot,DB_RATING,dbroot,DB_TMP(DB_RATING),RENAME_EXCHANGE);
        REFree(re_rate);
    }
    do{//name
        fileFD      fi      =openat(dbroot,DB_RATING,O_RDONLY);
        fileFD      fo      =openat(dbroot,DB_TMP(DB_RATING),O_WRONLY|O_TRUNC|O_CREAT);
        FILE*       dbi     =fdopen(fi,"wt");
        FILE*       dbo     =fdopen(fo,"wt");
        str tmp;
        h_RE        re_name =RENewByColumns(NBCOL_NAME,
                                ANY,
                                ANY,
                                ANY,
                                ANY,
                                ANY,
                                (tmp=RECompileTemplate(PARTIAL("%s"),1,titletag))
                            );
        h_Buffer    buf     =BufferNew(2);
        BOOL        done    =FALSE;

        free(tmp);
        BufferStream_GetLine(buf,dbi);
        BufferStream_Write(buf,dbo);

        BOOL done=FALSE;
        while(BufferStream_GetLine(buf,dbi)>0){
            if(REMatch(buf->buf_head,re_name)&&!done){
                str             old     =(str)BufferMove(buf);
                h_table_string  old_t   =table_string_bind2(old,"\t");
                h_table_string  old_tup =table_string_bind2(
                                            old_t->table[5],
                                            ","
                                        );
                int tuplen=0,nbelem=0;
                str loc=NULL;
                for(int i=0;i<old_tup->nb_element;++i){
                    if(strcmp(old_tup->table[i],titletag)==0){
                        loc=old_tup->table[i];
                    }else{
                        tuplen+=strlen(old_tup->table[i]);
                        nbelem++;
                    }
                }

                if(nbelem)
                    tuplen+=nbelem;
                else{
                    table_string_unbind(old_tup,",");
                    table_string_unbind(old_t,"\t");
                    table_string_liberer(old_tup);
                    table_string_liberer(old_t);
                    free(old);
                    done=TRUE;
                    goto _again;
                }
                
                str nouv=(str)xcalloc(nbelem+1,sizeof(char));
                
                for(int i=0;i<old_tup->nb_element;++i){
                    if(old_tup->table[i]==loc)
                        continue;
                    strcat(nouv,old_tup->table[i]);
                    strcat(nouv,",");
                }

                if(*(nouv+strlen(nouv)-1)==',')
                    *(nouv+strlen(nouv)-1)='\0';

                table_string_unbind(old_tup,",");
                loc=old_t->table[5]-1;
                table_string_unbind(old_t,"\t");
                *loc='\0';

                BufferPrintf(buf,"%s\t%s",2,old,nouv);
                BufferStream_Write(buf,dbo);

                free(nouv);
                table_string_liberer(old_tup);
                table_string_liberer(old_t);
                free(old);
                done=TRUE;
            }else{
                BufferStream_Write(buf,dbo);
            }
            _again:
        }

        BufferFree(buf);
        fclose(dbi);fclose(dbo);
        close(fi);close(fo);
        renameat2(dbroot,DB_NAME,dbroot,DB_TMP(DB_NAME),RENAME_EXCHANGE);
        REFree(re_name);

    }while(0);
    
    return TRUE;
}


