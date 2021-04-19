
#ifndef __UTIL_H
#define __UTIL_H

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "debug_include.h"
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//								TYPES USUELS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
typedef char*      str;
typedef const str  cstr;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//								ENTIERS ENUMERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/**
 * @brief  	type t_fichier est une reference au type de base de donnees utilisee 
 * 			presentement. Type opaque de enum_t_fichier
 */
typedef enum tag_fichier{
	tag_fichier_ERREUR = 0, 
	tag_fichier_TITRE, 
	tag_fichier_PERSONNE, 
	tag_fichier_COTE,
	tag_resultat_conjoint
} t_tag_fichier;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//						MACROFONCTIONS UTILITAIRES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/**
 * @brief  
 * @note   
 * @retval 
 */
#define ptimestamped(str,...){\
    time_t time=time(NULL);\
    struct tm* time_tm=gmtime(&time);\
    fprintf(\
		DBG_PIPE,\
		"[%d:%d:%d] "str,\
		time_tm->tm_hour,\
		time_tm->tm_min,\
		time_tm->tm_sec,\
		__VA_ARGS__\
	);\
}
//------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @retval 
 */
#define perrno(str,...) \
    fprintf(DBG_PIPE,"{ errno  :  %d  :  %s }  "str,errno,strerror(errno),__VA_ARGS__)
//------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @retval 
 */
#define perrno_if_not_true(cnd,str,...) if(cnd!=TRUE){\
    fprintf(DBG_PIPE,"{ errno  :  %d  :  %s }  "str,errno,strerror(errno),__VA_ARGS__);\
}
//------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @retval 
 */
#define perror_die(str,...) {\
    time_t tim=time(NULL);\
    struct tm* time_tm=gmtime(&tim);\
    fprintf(DBG_PIPE,"[%d:%d:%d]-{ %d  :  %s }  "str"\n",\
        time_tm->tm_hour,\
        time_tm->tm_min,\
        time_tm->tm_sec,\
        errno,\
        strerror(errno),\
        __VA_ARGS__\
    );\
    exit(EXIT_FAILURE);\
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#endif