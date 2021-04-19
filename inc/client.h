/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*
	Projet		: ELE216 - Laboratoire 1
	Date		: 15 fevrier 2021
	Par 		: Gabriel Gandubert et Hugo Cusson-Bouthillier
	Definition	: Encapsulation du type d'operation et des criteres recu par le
					programme principal.

	Contient:
		TYPES:
			Declaration de la structure opaque t_operation et son handle, pt_operation.

		FONCTIONS:
			operation_creer				:	Creation d'une structure des operations.
			operation_liberer			:	Liberation d'une strcuture des operations.
			operation_get_critere_i		:	Lecture d'un critere specifique de la 
											structure interne.
			operation_get_critere		:	Lecture de la structure des criteres.
			operation_get_taille_crit	:	Lecture du nombre d'element pour un critere
											specifique.
			operation_get_t				:	Lecture de l'element T de la structure des
											operations.
			operation_get_type_titre	:	Lecture de l'element type_tire de la structure
											des operations.
			operation_set_t				:	Modificateur de l'element T de la structure
											des operations.
			operation_set_critere		:	Modificateur de la structure des criteres
											de la structure des operations.
			operation_set_type_titre	:	Modificateur de l'element type_titre de la
											structures des operations.
			serialiser_operation		:	Transforme la struture des operations en
											chaine de caracteres.
			deserialiser_operation		:	Transformation de la chaine de caractere
											serialisee en structure des operations.

			
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef __OPERATION_H
#define __OPERATION_H

#include <stdio.h>
#include <stdlib.h>

#include "critere.h"
#include "fstr.h"
#include "util.h"
#include "debug_include.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*									CONSTANTES             								*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief  constante de l'operande des types d'operations acceptee par le programme.
 */
#define CHAR_TYPE_OPERATION (const char) 'T'
/**
 * @brief  constantes des types d'operations acceptees par le programme.
 */
#define RECHERCHE (type_alias) 'S'	// Search
#define RETRAIT (type_alias) 'R'	// Remove
#define AJOUT (type_alias) 'A'		// Add

/**
 * @brief  constantes des types de sorties possible du programme.
 */
#define OPERATION_SUCCESS (unsigned int) 0
#define EMPTY_ARGUMENT (unsigned int) 1
#define EMPTY_POINTER (unsigned int) 2
#define GENERAL_ERROR (unsigned int) 3
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/




/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*								Definitions de types       								*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief  : Structure encapsulant les operations et les criteres passes en argument a 
 *			 l'executable.
 */
typedef struct operation t_operation, *pt_operation;

// Type de l'operation recu par le programme.
typedef char type_alias;
struct operation
{
	type_alias T;
	pt_critere critere;
};
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/




/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							Debut des fonctions publiques								*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief  Creation d'une structure pour l'operation exigee.
 * @note   HLR** : 
 * @retval Un pointeur sur la structure creee et vide. Sil echoue, renvoie NULL.
 */
pt_operation operation_creer(void);

//----------------------------------------------------------------------------------------

/**
 * @brief  Liberation de la structure de l'operation exigee ainsi que le(s) critere(s) 
 *		   contenu(s).
 * @note   HLR05 : Allocation de la memoire d'une structure pour une operation.
 * @retval None
 */
extern void operation_liberer(void* oper);

//----------------------------------------------------------------------------------------

/**
 * @brief  Retourne un element de la structure des criteres.
 * @retval La chaine de caractere de l'element demandee. 
 *		   Si l'element n'existe pas, retourne NULL.
 */
const char* operation_get_critere_i(pt_operation oper, t_tag_critere tag, unsigned int i);

//----------------------------------------------------------------------------------------

/**
 * @brief  Retourne le pointeur des criteres. Si l'une des deux structure n'est pas
 * 		   initialisee, la fonction retourne un pointeur NULL.
 * @retval Pointeur vers la structure de critere. En cas d'erreur, retourne NULL.
 */
const pt_critere operation_get_critere(pt_operation oper);

//----------------------------------------------------------------------------------------

/**
 * @brief  Retourne la taille du premier tableau du critere selectionnee.
 * @retval Retourne la taille de l'element demandee. En cas d'erreur, retourne 0.
 */
unsigned int operation_get_taille_crit(pt_operation const oper, t_tag_critere tag);

//----------------------------------------------------------------------------------------

/**
 * @brief  recupere la valeur du type d'operation donnee par l'utilisateur.
 * @retval Retourne le type d'operation voulu dans la structure. 
 * 		   si la structure n'existe pas, la fonction retourne 0.
 */
const type_alias operation_get_t(pt_operation oper);

//----------------------------------------------------------------------------------------

/**
 * @brief  Lecture du type de titre de la structure.
 * @retval La chaine de caractere du type de titre de la structure. 
 *		   Si l'element n'existe pas, retourne NULL.
 */
const char* operation_get_type_titre(pt_operation oper);

//----------------------------------------------------------------------------------------

/**
 * @brief  	Changement du type d'operation de la structure des operations.
 * @note   	HLR06 : Mutateur de la structure des operations.
 * @retval	En cas de reussite, retourne 0. 
 *			En cas de pointeur de structure vide, retourne 1.
 *			En cas d'element vide, retourne 2.
 */
unsigned int operation_set_t(pt_operation operation, type_alias T);

//----------------------------------------------------------------------------------------

/**
 * @brief  	Changement du pointeur de critere de la structure des operations.
 * @note   	HLR06 : Mutateur de la structure des operations.
 * @retval	En cas de reussite, retourne 0. 
 *			En cas de pointeur de structure vide, retourne 1.
 *			En cas d'element vide, retourne 2.
 */
unsigned int operation_set_critere(pt_operation operation, pt_critere critere);

//----------------------------------------------------------------------------------------

/**
 * @brief  	Changement du type de titre de la structure des operations.
 * @note   	HLR06 : Mutateur de la structure des operations.
 * @retval	En cas de reussite, retourne 0. 
 *			En cas de pointeur de structure vide, retourne 1.
 *			En cas d'element vide, retourne 2.
 */
unsigned int operation_set_type_titre(pt_operation oper, char* type_titre);

//----------------------------------------------------------------------------------------

/**
 * @brief  	Transformation de la structure des operations en chaine de caractere. Chaque
 *			element et chaque critere est precedee d'un indicateur pour le replacer dans
 *			le meme ordre par une fonction de deserialisation.
 * @retval	En cas de reussite, retourne la chaine de caractere resultante. 
 *			En cas d'erreur, elle retourne un pointeur NULL.
 */
str serialiser_operation(pt_operation oper);

//----------------------------------------------------------------------------------------

/**
 * @brief  	Transformation d'une chaine de caractere provenant la fonction 
 *			serialiser_operation en une structure d'operation. 
 * @retval	En cas de reussite, retourne la structure des operations resultante. 
 *			En cas d'erreur, elle retourne un pointeur NULL.
 */
pt_operation deserialiser_operation(cstr serie);

//----------------------------------------------------------------------------------------

#endif
