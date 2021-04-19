/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*
	Projet		: ELE216 - Laboratoire 1
	Date		: 21 janvier 2021
	Par 		: Gabriel Gandubert et Hugo Cusson-Bouthillier
	Definition	: Encapsulation des criteres recu par le programme principal

	Contient:
		TYPES:
			- structure opaque t_table_string et son handle h_table_string
			- t_fp_foreach: signature de foncteur pour les fonctions de type foreach

		FONCTIONS PUBLIQUES:
			- creer_table_string    : constructeur de t_table_string
            - liberer_table_string  : destructeur de t_table_string          
            - get_nb_elements       : observatrice de taille de table
            - get_string_a_index    : observatrice d'un element de table
            - table_string_foreach  : observatrice iterative
*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Header file.
#include "table_string.h"
#include "util.h"
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							TYPES ET STRUCTURES OPAQUES     							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief  	type t_table_critere est une table de strings utilisee dans l'implementation 
 * 			de la structure opaque t_critere
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*					IMPLEMENTATION FONCTIONS PUBLIQUES         							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
h_table_string table_string_creer( cstr s_tuple, cstr token){
/**
	 *	La structure sousjascente de cette table de references est en realite un seul 
	 *  tableau de char contenant des '\0' entre les differentes strings. 
	 *  Le debut de la premiere string est stoque dans le premier pointeur du 
	 *  t_table_critere afin de pouvoir liberer sa memoire. Ainsi, la structure retournee 
	 *  contient des pointeurs sur toutes les strings, mais aussi un pointeur sur le 
	 *  tableau sousjascent.
	 */

	str 			buffer	        =NULL;				//pour copier la string en argument
	str			    str_extraite    =NULL;              //usage de strtok
	h_table_string  nouveau         =NULL;				//pour la structure de retour
	int             incr=0;
	int 			i=1;

	if( !s_tuple || !*s_tuple || !token ){ 
        //validation des entrees
		return NULL;
	}
	incr=strlen(token);

    //on alloue la memoire pour la copie du segment, renvoie null si echoue
	if(  !(  buffer = calloc(strlen(s_tuple)+2,sizeof(char))  ) ){
		return NULL;
	}
	strcpy(buffer,s_tuple);

    //creation de la struct de retour et initialisation
    if( !( nouveau = (h_table_string)malloc(sizeof( t_table_string ) ) ) ){
        //sur echer, on libere tout ce quon a cree et on renvoie null
        free(buffer);
        buffer=NULL;
        return NULL;
    }

    nouveau->nb_element =0;
    nouveau->table      =NULL;
	str max=buffer+strlen(buffer);
	str_extraite=strstr(buffer,token);
	while(str_extraite&&*str_extraite){
		++i;
		str_extraite+=incr;
		if(str_extraite>max){

			printf("error table_string_creer: buffer overflow.");

		}

		str_extraite = strstr(str_extraite,token);
	}

	if(i==0)i=1;
	str* table=NULL;
	if(!(table=malloc(i*sizeof(str)))){
		free(buffer);
		free(nouveau);
		return NULL;
	}
	
	table[0]=buffer;
	++(nouveau->nb_element);

	str_extraite=strstr(buffer,token);
	while(str_extraite){
		for(i=0;i<incr;++i){	
			*str_extraite='\0';
			++str_extraite;
			if(str_extraite>max){
				printf("error table_string_creer: buffer overflow.");
			}
		}
		if(str_extraite>max){
			printf("error table_string_creer: buffer overflow.");
		}
	
		table[nouveau->nb_element]=str_extraite;
		nouveau->nb_element++;
		str_extraite=strstr(str_extraite,token);
	}
	nouveau->table=table;
	return nouveau; 
}
unsigned int table_string_bind(h_table_string dest,str buf,cstr sep){
	int occ;
	str* oldtable;
	str  oldfirst;

	if(!dest||!buf||!sep||dest->table[0])
		return FALSE;
	if((occ=count_occurence(sep,buf))<0)
		return FALSE;

	if(!dest->nb_element){
		if(!dest->table)
			dest->table=calloc(occ+1,sizeof(char*));
		else{
			oldfirst=dest->table[0];
			oldtable=dest->table;
			if(!(dest->table=realloc(dest->table,(occ+1)*sizeof(char*)))){
				dest->table=oldtable;
				dest->table[0]=oldfirst;
				return FALSE;
			}
			free(oldfirst);
		}
	}else{
		if(!(dest->table=calloc(occ+1,sizeof(char*))))
			return FALSE;
	}
	tokenize(buf,dest->table,sep);
	dest->nb_element=occ+1;
	return TRUE;
}
h_table_string table_string_bind2(str buf,cstr sep){
	int occ;
	h_table_string table;
	if(!buf||!sep)
		return FALSE;

	if((occ=count_occurence(*buf,sep)<0))
		return FALSE;
	
	if(!(table=calloc(1,sizeof(t_table_string))))
		return FALSE;
	
	if(!(table->table=calloc(occ+1,sizeof(char*))))
		free(table);
		return NULL;
	
	tokenize(buf,table->table,sep);
	return TRUE;
}

unsigned int table_string_unbind(h_table_string table,cstr sep){
	for(int i=0;i<table->nb_element-1;++i){
		str csor=table->table[i]+strlen(table->table[i]);
		strcpy(table->table[i],sep);
	}
	memset(table->table,0,table->nb_element*sizeof(char*));
	
	return TRUE;
}
unsigned int table_string_unbind2(str* dest,h_table_string table,cstr sep){
	if(!dest||*dest)
		return FALSE;
	for(int i=0;i<table->nb_element-1;++i){
		str csor=table->table[i]+strlen(table->table[i]);
		strcpy(table->table[i],sep);
	}
	dest=table->table[0];
	memset(table->table,0,table->nb_element*sizeof(char*));
	return TRUE;
}
//-------------------------------------------
void table_string_liberer(void* table_str){
	/**
	 * Strategie: On doit faire 2 choses seulement.
	 * 	1- on libere le data sousjascent accessible via le premier pointeur du array
	 *  2- on libere la table de pointeur.
	 * Pour plus dinformation, voir implementation de creer_table_critere.
	 */
	h_table_string s=Cast(table_string,table_str);

	if(s){
        if(s->table){
            free(s->table[0]);  //libere le data sousjascent
            s->table[0]=NULL;
            free(s->table);     //libere linterface
            s->table=NULL;
        }
	}
	free(s);//libere la table de pointeurs
	s=NULL;
}
//-------------------------------------------
unsigned int table_string_get_taille(h_table_string table_str){
	return (
		table_str
		?	table_str->nb_element
		:	0
	);
}
//-------------------------------------------
cstr table_string_get_element_n(h_table_string table_str, unsigned int n){
	return (	
		( table_str && ((table_str->nb_element) > n) )
		?	table_str->table[n]
		:	NULL	
	);
}

unsigned int table_string_foreach(	const h_table_string 	table_str	,
									const fp_val_ctx		f_do		, 
									void					*f_contexte ){
	if( !table_str || !table_str->table || !f_do ) return 0; //validation des args

	for( int i=0; i<table_str->nb_element; ++i) 
		f_do( table_str->table[i], f_contexte ); 				 //iteration

	return table_str->nb_element;							//retourne le nombre d'iter
}
//-------------------------------------------
unsigned int table_string_contient(	const h_table_string 	table_str	,
									const fp_comp			f_comparer	,
									cstr				f_str2		){
	if( !table_str || !table_str->table || !f_comparer || !f_str2 ) 
		return 0; //validation des args

	unsigned int retour=0;

	for( int i=0; i<table_str->nb_element; ++i) 
		retour|=f_comparer(table_str->table[i], f_str2 );	 //iteration

	return retour;							 //retourne le nombre d'iter
}
//-------------------------------------------

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
