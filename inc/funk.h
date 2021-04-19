#ifndef __FUNK_H
#define __FUNK_H
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*
	Projet		: ELE216 - Laboratoire 1
	Date		: 21 janvier 2021
	Par 		: Gabriel Gandubert et Hugo Cusson-Bouthillier
	Definition	: Module de definition des foncteurs d'accès publique au client

    Accronymes:
        - Designer:     
            le parti qui programme la fonction du type correspondant au foncteur

        - Appelant:     
            le parti qui appelle la fonction du type correspondant au foncteur

        - ctx / contexte :
            un contexte applicatif(variable) construit et typé par le designer 
            Le type est connu/inconnu de l'appelant et celui ci ne connait pas 
            l'adresse de cette variable avant que le designer lui en informe. 
            le designer passe le contexte a l'appelant pour qu'il appelle le 
            foncteur dans le CONTEXTE de cette variable.

        - key / cle:          
            Une variable d'adresse connue de l'appelant et connue/inconnue
            du designer dont le type est connu du designer et connu ou non de 
            l'appelant. Cette variable identifie(indexe) souvent un element d'une 
            collection présentant une interface d'opérations virtuelles telles que 
            foreach.

        - val / valeur:       
            Une variable d'adresse connue de l'appelant et connue/inconnue du 
            designer. Son type est connu/inconnu de l'appelant et CONNU 
            du designer. Réfère souvent aux elements d'une collection d'objets de 
            type abstraits a l'appelant mais connus du designer.

        - dtor / destructeur: 
            la methode reimplementant free pour l'objet passé a l'appel

        - comp / comparateur: 
            l'action comise est une comparaison abstraite a l'appelant

        - lhs: LEFT-HAND-SIDE.
        - rhs: RIGHT-HAND-SIDE.
*           
*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "util.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                            FONCTEURS TYPÉS
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief   type t_table_critere est une table de strings utilisee dans l'implementation 
 *          de la structure opaque t_critere
 */
typedef unsigned int(*t_fp_foreach  )(const char*, void*);

/**
 * @brief   virtuelle de comparaison de deux objets de memes types
 * @param   1: objet a comparer a (2)
 * @param   2: objet a comparer a (1)
 * @retval  TRUE si la comparaison virtuelle { *1 OP *2 } retourne vrai, FALSE sinon
 */
typedef unsigned (*fp_comp)(void*,void*);

/**
 * @brief  virtuelle de destruction d'un objet dynamique de type abstrait a l'appelant
 * @param   1: l'objet a detruire
 */
typedef void (*fp_dtor)(void*);

/**
 * @brief  virtuelle d'operation impliquant une VALEUR(type abstrait a l'appelant, 
 *          passé par l'appelant) et un CONTEXTE(type abstrait a l'appelant, passé par le 
 *          designer)
 * @param   1: la VALEUR donnée par l'appelant
 * @param   2: le CONTEXTE spécifié a l'appelant par le designer
 */
typedef void (*fp_val_ctx)(void*,void*);

/**
 * @brief       foncteur pour une fonction de type contexte cle valeur appelée sur une 
 *          collection abstraite contenant des VALEURS indexées par une CLE, 
 *          dans un CONTEXTE donné par le designer
 * @param  1: void* context:        le contexte d'execution de type abstrait a l'appelant 
 *                              et concret au designer
 * @param  2: const char* const cle: la cle de la paire donnee a la fonction par 
 *              l'iteratrice
 * @param  3: void* valeur: la valeur de la paire donnee a la fonction par l'iteratrice.
 *              son type est implicite a l'appelant l'implementant.
 * @retval retourne void
 */
typedef void (*fp_ctx_cle_val)(void*,cstr ,void*);

/**
 * @brief       foncteur pour une fonction de type contexte cle valeur appelée sur une 
 *          collection abstraite contenant des VALEURS indexées par une CLE, 
 *          dans un CONTEXTE donné par le designer et retournant un statut de sortie 
 *          a l'appelant
 * @param  1: void* context: le contexte d'execution sous la forme libre a l'implementatio
 *              et de type implicite a l'appelant.
 * @param  2: const char* const cle: la cle de la paire donnee a la fonction par 
 *              l'iteratrice
 * @param  3: void* valeur: la valeur de la paire donnee a la fonction par l'iteratrice.
 *              son type est implicite a l'appelant l'implementant.
 * @retval retourne EXIT_SUCCESS dans le cas d'un succes, EXIT_FAILURE sinon.
 */
typedef int  (*fp_ctx_cle_val_return_exit)(void*, const char* const,void*);



#endif