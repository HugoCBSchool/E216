#include "util/fstr.h"


//===============================================================================
bool __fstr_cashvariade_formater_dest( str* dst, cstr fmt, ... ){
    str     buf         =NULL;
    str     cash_tag    =NULL;
    str     cash_value  =NULL;
    va_list variade;
    
    //make sure args are ok
    if( !fmt || !dst  )
        return FALSE;

    //start the variade list
    va_start(variade,fmt);

    //if the first variaadic is null or the copy of the template fails we exit in error
    if( !( cash_tag = va_arg(variade,str) ) || !( buf = strdup(fmt) )  ) {
        va_end(variade);
        return FALSE;   
    }
    
    //for as long as cash_tag is not the variade end-sentinel
    while( strcmp(cash_tag,CASH_VARIADE_SENTINEL)!=0 ){
        //we have a cash_value to unload
        //  if its not null, we replace the tag by the value
        //    if it worked we get the next variadic 
        //    which should be the end sentinel or another cashtag
        //      if its not null we loop again
        //otherwise failure
        if( !(      ( cash_value = va_arg(variade,str) ) 
                &&  fstr_remplacer_dest(&buf,buf,cash_tag,cash_value)
                &&  ( cash_tag   = va_arg(variade,str) )
            )
        ){
            free(buf);
            va_end(variade);
            return FALSE;
        }
    }

    //make sure no memleak in the population of destination and populate
    if(*dst)
        free(*dst);
    
    *dst=buf;
    va_end(variade);
    return TRUE;
}
str  __fstr_cashvariade_formater_make(           cstr fmt, ... ){
    str     buf         =NULL;
    str     cash_tag    =NULL;
    str     cash_value  =NULL;
    va_list variade;
    
    //make sure args are ok
    if( !fmt )
        return NULL;

    //start the variade list
    va_start(variade,fmt);

    //if the first variaadic is null or the copy of the template fails we exit in error
    if( !( cash_tag = va_arg(variade,str) ) ){
        va_end(variade);
        return NULL;
    }
    if(!( buf = strdup(fmt) )  ){  
        va_end(variade);
        return NULL;   
    }

    //for as long as cash_tag is not the variade end-sentinel
    while( strcmp(cash_tag,CASH_VARIADE_SENTINEL)!=0 ){
        
        //we have a cash_value to unload
        //  if its not null, we replace the tag by the value
        //    if it worked we get the next variadic 
        //    which should be the end sentinel or another cashtag
        //      if its not null we loop again
        //otherwise failure
        if(     ( cash_value = va_arg(variade,str) )==NULL
            ||  !fstr_remplacer_dest(&buf,buf,cash_tag,cash_value)
        ){
            va_end(variade);
            free(buf);
            return NULL;
        }
        if(!( cash_tag   = va_arg(variade,str) )){
            va_end(variade);
            free(buf);
            return NULL;
        }
    }
    va_end(variade);
    return buf;
}
//===============================================================================
str  fstr_remplacer_make( cstr src, cstr old_str, cstr new_str){
	str buf=NULL;
    if(!fstr_remplacer_dest(&buf,src,old_str,new_str))
        return NULL;

	return buf;
}
bool fstr_remplacer_dest( str* dst, cstr src, cstr old_str, cstr new_str){
    if(!( new_str && src && old_str && *src && *old_str && dst)){
		return FALSE;
	}
	str nouv=src;
	int   sz=0;
	h_list ls=NULL;
	
	if(!(ls=list_new(NULL,NULL)))
		return FALSE;

    sz=strlen(old_str);
	while((nouv=strstr(nouv,old_str))!=NULL){
		list_push_tail(ls,nouv);
        nouv+=sz;
	}
	
	if(!list_get_nb_elem(ls)){
		list_free(ls);
		return FALSE;
	}

	sz=strlen(src)+1+( strlen(new_str) - strlen(old_str) )*list_get_nb_elem(ls);
	if(!( nouv=calloc(sz,sizeof(char)) )){
		list_free(ls);
		return FALSE;
	}
	

	for(	char *head=list_itor_head(ls), *nxt=list_itor_next(ls),*dest=nouv,*h_src=src;
			head;
			head=nxt,nxt=list_itor_next(ls)
	){
        while(h_src<head){
            *dest=*h_src;
            ++dest;
            ++h_src;
        }
        h_src+=strlen(old_str);
        memcpy(dest,new_str,strlen(new_str));
        dest+=strlen(new_str);
		if(!nxt){
			memcpy(dest,h_src,sizeof(char)*(strlen(h_src)+1));
		}
	}

	list_free(ls);
    ls=NULL;
    if(*dst){
        free(*dst);
        *dst=NULL;
    }
    *dst=nouv;
	return TRUE;
}
//===============================================================================
bool  fstr_flatten_table_string_dest(str* dest, h_table_string table, cstr sep){
    str buf=fstr_flatten_table_string_make(table,sep);
    if(!buf)
        return FALSE;
    
    if(*dest)
        free(*dest);
    *dest=buf;
    return TRUE;
}
str   fstr_flatten_table_string_make(           h_table_string table, cstr sep){
    str  buf     =NULL;
    str  tail    =NULL;
    bool success =TRUE;

    if( !table || !sep || !table_string_get_taille(table) )
        return NULL;

    success&=fstr_remplacer_dest(
        &buf,$(dst)$(tail) ,
        $(dst),(cstr)table_string_get_element_n(table,0)
    );
    success&=fstr_remplacer_dest(
        &tail,$(op)$(dst)$(tail),
        $(op),sep
    );

    for( int i= 1; i<table_string_get_taille(table); ++i){
        success&=fstr_remplacer_dest(
            &buf,buf, $(tail), tail 
        );
        success&=fstr_remplacer_dest( 
             &buf,buf, $(dst), (cstr)table_string_get_element_n(table,i)
        );
    }
    success&=fstr_remplacer_dest(&buf, buf , $(tail) , "");

    free(tail);

    if(!success){
        free(buf);
        buf=NULL;
    }

    return buf;
}
//===============================================================================


bool  fstr_flatten_linklist_dest( str* dest, h_list list, cstr sep){
    
    str buf=NULL;
    str tail=NULL;

    if( !list || !sep || !list_get_nb_elem(list) )
        return FALSE;

    if(!fstr_remplacer_dest(&buf,"${dst}${tail}","${dst}",list_itor_head(list)))
        return FALSE;

    if(!fstr_remplacer_dest(&tail,"${op}${dst}${tail}","${op}",sep)){
        free(buf);
        return FALSE;
    }

    for( str i = list_itor_next(list); i; i=list_itor_next(list) ){
        
        if(!fstr_remplacer_dest(&buf,buf,  "${tail}", tail )){
            free(buf);
            free(tail);
            return FALSE;
        }

        if(!fstr_remplacer_dest(&buf,  buf, "${dst}", i )){
            free(buf);
            free(tail);
            return FALSE;
        }
    }

    if(!fstr_remplacer_dest( &buf ,buf ,  "${tail}" , "")){
        free(buf);
        free(tail);
        return FALSE;
    }

    free(tail);

    if(*dest)
        free(*dest);

    *dest=buf;

    return TRUE;
}
str   fstr_flatten_linklist_make(            h_list list, cstr sep){
    str buf=NULL;
    return (fstr_flatten_linklist_dest(&buf,list,sep)?buf:NULL);
}
bool  fstr_flatten_array_dest(    str* dest, cstr*  list, uint sz, cstr sep){
    str buf=NULL;
    str tail=NULL;

    if( !list || !sep || !sz)
        return FALSE;

    for(int i=0;i<sz;++i){
        if(!list[i])
            return FALSE;
    }

    if(!fstr_remplacer_dest(&buf,"${dst}${tail}","${dst}",*list))   
        return FALSE;

    if(!fstr_remplacer_dest(&tail,"${op}${dst}${tail}","${op}",sep)){
        free(buf);
        return FALSE;
    }

    for( int i=0; i<sz;++i ){
        
        if(!fstr_remplacer_dest(&buf, buf, "${tail}", tail )){
            free(buf);
            free(tail);
            return FALSE;
        }

        if(!fstr_remplacer_dest( &buf, buf, "${dst}", list[i] )){
            free(buf);
            free(tail);
            return FALSE;
        }
    }

    if(!fstr_remplacer_dest( &buf,buf , "${tail}" , "")){
        free(buf);
        free(tail);
        return FALSE;
    }

    free(tail);

    if(*dest)
        free(*dest);

    *dest=buf;

    return TRUE;
}
str   fstr_flatten_array_make(               cstr*  list, uint sz, cstr sep){
    str buf=NULL;
    return (fstr_flatten_array_dest(&buf,list,sz,sep)?buf:NULL);
}
/*
bool  fstr_gawkfmt_interval_dest(str* dest, cstr column, cstr interval){
    char *buf=NULL , *low=NULL , *high=NULL;
    h_table_string table=NULL;
    int cpt;

    if(!column||!dest||interval)
        return FALSE;
    
    if(!( table=table_string_creer(interval,":") ))
        return FALSE;

    if( (cpt=table_string_get_taille(table)) < 1 || cpt > 2 ){
        table_string_liberer(table);
        return FALSE;
    }

    if( cpt==2 ){
        if( (high=(str)table_string_get_element_n(table,1)) ){
            cpt=0;
            for(str p=high;*p;++p) cpt+=isdigit(*p);

            if(!cpt){
                table_string_liberer(table);
                return FALSE;
            }
        }else{
            table_string_liberer(table);
            return FALSE;
        }
    }
    if( (low=(str)table_string_get_element_n(table,0)) ){
        cpt=0;
        for(str p=low;*p;++p) cpt+=isdigit(*p);

        if(!cpt){
            table_string_liberer(table);
            return FALSE;
        }
    }else{
        table_string_liberer(table);
        return FALSE;
    }
    
    if(!fstr_cashvariade_formater_dest(&buf,
            "(" $(col) "<=" $(higher) ") && (" $(col) ">=" $(lower) ")",
            $VARIADE(
                $(lower)     , low,
                $(higher)    , high,
                $(col)       , column
            )
        )
    ){   
        table_string_liberer(table);
        return FALSE;
    }

    if(*dest)
        free(*dest);
    
    table_string_liberer(table);
    *dest=buf;

    return TRUE;
}
str   fstr_gawkfmt_interval_make(           cstr column, cstr interval){
    str buf=NULL;
    return (fstr_gawkfmt_interval_dest(&buf,column,interval)?buf:NULL);
}/*
str   fstr_gawkfmt_pattern_critere( pt_critere critere, t_tag_fichier tag){
    str   buf     =NULL;
    str   buf2    =NULL;
    h_list  ls_str  =NULL;
    h_list  ls_str_annee=NULL;
    bool    success=TRUE;

    if(!critere)
        return NULL;

    switch(tag){

        case tag_fichier_TITRE:
            {
                if(!( ls_str=list_new(free,NULL)) )
                    break;

                for(int i=0;i<critere_get_taille(critere,tag_titre);++i){
                    success&=list_push_head(
                        ls_str,
                        fstr_remplacer_make(
                            "("   $(dest)    ")",
                            $(dest),
                            critere_get_element_i(critere,tag_titre,i)
                        )
                    );
                }

                success&=( (buf=fstr_flatten_linklist_make(ls_str,"|")) != NULL );

                list_clear(ls_str);

                if(critere_get_taille(tag_annee,critere)){

                    for(int i=0;i<critere_get_taille(critere,tag_annee);++i)
                        success&=list_push_head(
                            ls_str,
                            fstr_gawkfmt_interval_dest(
                                &buf2,
                                "$6",
                                critere_get_element_i(critere,tag_annee,i)
                            )
                        );             

                    success&=(( buf2=fstr_flatten_linklist_make(ls_str,"||") )!=NULL);

                    list_clear(ls_str);

                }
				cash_bracket_format_paires_dest(&buf,3
					$(id_op)			"\t"
					$(critere_titre)	"\t"
					$(critere_cote)		"\t",
					$(id_op),"recherche"

				)
                success&=cash_bracket_format_paires_dest(&buf
                    ,   "("    
                                "$1~/"  $(cnd_titre)  "/"   
                        ")&&("    
                                "$6~/"  "^[0-9]{4}"     "/"   
                        ")&&("      
                                $(cnd_date)         
                        ")"
                    ,   $(cnd_titre)  ,   buf
                    ,   $(cnd_date)   ,   buf2?buf2:"$0"
                );

                list_free(ls_str);
                
                free(buf2);

                if(!success){
                    free(buf);
                    buf=NULL;
                }

            }break;

        case tag_fichier_COTE:
            {
                str cote=strdup("-1.0");

                for(int i=0;i<critere_get_taille(tag_cote,critere);++i){

                    int   cpt =0;
                    str p   =critere_get_element_i(critere,tag_cote,i);
                    
                    for(str pt=p; ++pt; *pt )
                        cpt+=isdigit(*pt);

                    if(strchr(p,'.')&&cpt==1){

                        if(atof(cote)<atof(p)){
                            free(cote);
                            cote=strdup(p);
                        }
                    }

                }

                if( atof(cote) >= 0.0 ){

                    if(!cash_bracket_format_paires_dest(&buf,
                        "("   "$2"  ">="  $(val)   ")",
                        $(val)    ,   cote
                    )){
                        free(buf);
                        buf=NULL;
                    }
                }

                free(cote);

            }break;

        case tag_fichier_PERSONNE:
            {
                int taille;

                if(!( taille=critere_get_taille(tag_personne,critere) ))
                    break;

                if(!( ls_str=list_new(free,NULL)) )
                    break;
                
                for(int i=0;i<taille;++i){
                    success&=list_push_head(
                        ls_str,
                        fstr_remplacer_make(
                            "("  $(dest) ")",
                            $(dest)   ,  critere_get_element_i(critere,tag_personne,i)
                        )
                    );
                }

                success&=( (buf=fstr_flatten_linklist_make(ls_str,"|"))!=NULL );

                list_free(ls_str);
                
                success&=cash_bracket_format_paires_dest(&buf,
                    "("  
                        "$2"  "~/"   $(tuple_noms)    "/"
                    ")",
                    $(tuple_noms),buf
                );

                if(!success){
                    free(buf);
                    buf=NULL;
                }
            }break;
    }
    
    return buf;
}*/

str ligne_formater(cstr ligne){
	if(!ligne||!strlen(ligne))
		return NULL;

	int nb_err=0;
	str nouv=NULL;

	for(str p=ligne+1; *p; ++p){
		if( *p=='\t' && *(p-1)=='\t' )
			++nb_err;
	}

	if(!( nouv=calloc( strlen(ligne)+1+(nb_err*strlen("N/C")),sizeof(char)) ))
		return NULL;

	for(char *p=ligne+1,*q=nouv; *p ; ++p){
		*q=*p;
		++q;
		if(*p=='\t' && *(p+1)=='\t'){
			strcpy(q,"N/C");
			q=q+3;
		}

		if(!(*(p+1)))
			*q='\0';
	}
	
	return nouv;
}