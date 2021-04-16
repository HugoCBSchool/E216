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

		FONCTIONS:
			- creer_table_string    : constructeur de t_table_string
            - liberer_table_string  : destructeur de t_table_string          
            - get_nb_elements       : observatrice de taille de table
            - get_string_a_index    : observatrice d'un element de table
            - table_string_foreach  : observatrice iterative
*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef _TABLE_STRING_H
#define _TABLE_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/util.h"

#include "util/funk.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							TYPES ET STRUCTURES OPAQUES     							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief  	type t_table_critere est une table de strings utilisee dans l'implementation 
 * 			de la structure opaque t_critere
 */
typedef struct table_string t_table_string,*h_table_string;
struct table_string{
    unsigned int    nb_element;
    str*          table;
};
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

inline int count_occurence(cstr ofthis,cstr inthis){
	int ret=0;
	if(!ofthis||!inthis)
		return -1;
	for(str csor=inthis;csor;csor=strstr(inthis,ofthis))
		++ret;
	return ret;
}
inline void tokenize(str this,str* array,cstr sep){
	array[0]=this;
	int i=0;
	int len=strlen(sep);
	str csormax=this+strlen(this);
	for(str csor=strstr(this,sep);csor;csor=strstr(csor,sep)){
		memset(csor,0,len*sizeof(char));
		if(len<=csormax-csor)
			array[++i]=(csor+len);
	}
}
inline void untokenize(str* array, int nmemb,cstr sep){
	for(int i=0;i<nmemb-1;++i){
		str csor=array[i]+strlen(array[i]);
		strcpy(array[i],sep);
	}
	memset(array,0,nmemb*sizeof(char*));
	
}



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							DECLARATIONS FONCTIONS PRIVEES    							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief  	permet de parser la string de criteres a tokeniser pour creer une 
 * 			nouvelle table de criteres dynamique
 * @param  	s_tuple	:	la string de criteres separes par des virgules
 * @param  	token	:	le token de delimitation du string au format tuple
 * @retval 	un nouveau tableau de criteres ou NULL si erreur
 */
h_table_string table_string_creer( cstr s_tuple, cstr token);
unsigned int table_string_bind(h_table_string dest,str buf,cstr sep);
h_table_string table_string_bind2(str buf,cstr sep);
unsigned int table_string_unbind(h_table_string table,cstr sep);
unsigned int table_string_unbind2(str* dest,h_table_string table,cstr sep);


/**
 * @brief  permet de liberer la plage de memoire dun t_table_critere
 * @param  table_str: la table a liberer
 * @retval None
 */
void table_string_liberer(h_table_string table_str);

/**
 * @brief  permet de savoir le nombre d'elements dans la table
 * @param  table_str: la table a observer
 * @retval le nombre d'elements, 0 si erreur
 */
unsigned int table_string_get_taille(h_table_string table_str);

/**
 * @brief  permet d'observer un element de la table a l'index donne
 * @param  table_str:   la table a observer
 * @param  index:       l'index de la string voulue
 * @retval une reference constante sur la string voulue, null si erreur
 */
cstr table_string_get_element_n(h_table_string table_str, unsigned int index);

/**
 * @brief  	Fonction permettant d'iterer sur tous les string de la structure en realisant 
 *          une routine d'execution donnee prennant en argument une reference constante 
 *          sur la string de l'iteration presente et une reference sur un
 *          contexte d'execution dont le type est implicite a l'appel de routine.
 * @param   table_str   :   une reference constante sur la table sur laquel iterer
 * @param   f_do        :   reference sur une fonction de type t_fp_foreach a appeler sur
 *                          chaque element de la table
 * 						    -   Elle prend en premier argument une string c-style constant
 * 							    pour recevoir le critere de l'iteration actuelle.
 * 						    -   Elle prend en second argument une reference void
 * 							    dont le type est implicite a l'implementation de la 
 *                              fonction appelante de sorte que celle-ci puisse manipuler 
 *                              son etat de fonctionnement et son retour.
 * @param	f_data	    :   l'adresse de la plage de memoire donnee en second parametre a 
 *                          la fonction f_do, dont la structure est implicite a 
 *                          l'implementation de l'argument fonctionnel pf. 
 *                          Elle peut etre nule, tant pour autant que la fonction pf ne 
 *                          tente pas d'y acceder. Sinon, la responsabilite de gestion de 
 *                          cette plage est celle de l'appelant de critere_foreach.
 * @retval 	Elle retourne le nombre diterations faites pour table_string_foreach
 * 			Elle retourne la somme des valeurs retournees pour table_string_foreach_add
 *			Elle retourne la reduction par OR de toutes les valeurs de retour
 * */
unsigned int table_string_foreach(	const h_table_string 	table_str	,
									const fp_val_ctx		f_do		, 
									void					*f_contexte	    );
									
/**
 * @brief  	Fonction permettant d'iterer sur tous les string de la structure en 
 *  		comparant avec une string passee en argument pour voir si la table contient
 * 			un string qui contient celle donnee en argument.
 * @param   table_str   :   une reference constante sur la table sur laquel iterer
 * @param   f_do        :   reference sur une fonction de type t_fp_foreach a appeler sur
 *                          chaque element de la table
 * 						    -   Elle prend en premier argument une string c-style constant
 * 							    pour recevoir le critere de l'iteration actuelle.
 * 						    -   Elle prend en second argument une seconde string
 * 								sur laquelle comparer.
 * @param	f_str2	    :   la seconde string avec comparer la collection.
 * @retval Elle retourne le nombre diterations faites
 */
unsigned int table_string_contient(	const h_table_string 	table_str	,
									const fp_comp			f_comparer	,
									cstr				    f_str2		);

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#endif