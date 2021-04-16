/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*
	Projet		: ELE216 - Laboratoire 1
	Date		: 10 fevrier 2021
	Par 		: Gabriel Gandubert et Hugo Cusson-Bouthillier
	Definition	: Comparaison des lignes de donnees des bases de donnees de films et des
				  criteres de recherche de l'utilisateur.

	Contient:

		FONCTIONS:
 
 			% Compare la ligne d'entree avec les criteres de titre recu et indique s'il
 			faut la garder.
			unsigned int comparer_donnee_titre_critere(
				h_donnee_titre const donnee,
				pt_critere const critere
			);
			% Compare la ligne d'entree avec les criteres de cote recu et indique s'il
			faut la garder.
			unsigned int comparer_donnee_cote_critere(  
			    h_donnee_cote   const donnee,
			    pt_critere      const critere
			);
			% Compare la ligne d'entree avec les criteres de nom recu et indique s'il faut
            la garder.
			unsigned int comparer_donnee_nom_critere(
			    h_donnee_nom    const donnee,
			    pt_critere      const critere
			);
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef __COMPARAISON_H
#define __COMPARAISON_H


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/util.h"
#include "util/table_string.h"
#include "modules/critere.h"
#include "modules/resultat.h"


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							Debut des fonctions publiques								*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief   Compare la chaine 1 avec la chaine 2 afin de determiner si la chaine 1 est
            comprise dans la chaine 2.
 * @param   str1:   chaine de caractere a trouver dans str2.
 * @param   str2:   chaine de caractere a verifier.
 * @retval  succes:     valeur booleen 1 = str1 est dans str2.
 *          echec:      valeur booleen 0 = str1 n'est pas dans str2.
 */
extern unsigned int str1_dans_str2(void* str1,void* str2);

/**
 * @brief   Compare la chaine 1 avec la chaine 2 afin de determiner si la chaine 2 est
            comprise dans la chaine 1.
 * @param   str1:   chaine de caractere a verifier.
 * @param   str2:   chaine de caractere a trouver dans str1.
 * @retval  succes:     valeur booleen 1 = str2 est dans str1.
 *          echec:      valeur booleen 0 = str2 n'est pas dans str1.
 */
extern unsigned int str2_dans_str1(void* str1,void* str2);

/**
 * @brief   Compare l'annee et l'interval afin de determiner si l'annee est comprise
            dans l'interval donne.
 * @param   interval: plage d'annee voulu separee par le caractere de separation.
 * @param   annee: l'annee donnee que nous voulons comparer.
 * @retval  succes:     valeur booleen 1 = annee est dans l'interval.
 *          echec:      valeur booleen 0 = annee n'est pas dans l'interval.
 */
extern unsigned int annee_dans_interval(void* interval,void* annee);

/**
 * @brief   Compare la cote et l'interval afin de determiner si la cote est comprise
            dans l'interval donne.
 * @param   interval: plage de cote voulu separee par le caractere de separation.
 * @param   cote: la cote donnee que nous voulons comparer.
 * @retval  succes:     valeur booleen 1 = cote est dans l'interval.
 *          echec:      valeur booleen 0 = cote n'est pas dans l'interval.
 */
extern unsigned int cote_dans_interval(void* interval,void* cote);


/**
 * @brief   Compare la ligne d'entree avec les criteres de titre recu et indique s'il faut
 *          la garder.
 * @note	HLR27: Analyse de l'argument -t et -a avec une ligne du fichier des titres.
 * @param   donnee: la ligne de donnees provenant de la base de donnee des titres de film.
 * @param   critere: les critere de recherche de l'utilisateur.
 * @retval  succes:     valeur booleen 1 = le titre recherche est dans la ligne.
 *          echec:      valeur booleen 0 = le titre recherche n'est pas dans la ligne.
 */
unsigned int comparer_donnee_titre_critere( 
    h_donnee_titre const donnee,
    pt_critere const critere
);

/**
 * @brief   Compare la ligne d'entree avec les criteres de cote recu et indique s'il faut
 *          la garder.
 * @note	HLR28: Analyse de l'argument -r avec une ligne du fichier des cotes.
 * @param   donnee: la ligne de donnees provenant de la base de donnee des cotes de films.
 * @param   critere: les critere de recherche de l'utilisateur.
 * @retval  succes:     valeur booleen 1 = la cote recherche ou superieur est dans la 
 *                      ligne.
 *          echec:      valeur booleen 0 = une cote inferieur a la cote voulue est dans la
 *                      ligne.
 */
unsigned int comparer_donnee_cote_critere(  
    h_donnee_cote   const donnee,
    pt_critere      const critere
);

/**
 * @brief   Compare la ligne d'entree avec les criteres de nom recu et indique s'il faut
 *          la garder.
 * @note	HLR26: Analyse de l'argument -p avec une ligne du fichier des noms.
 * @param   donnee: la ligne de donnees provenant de la base de donnee des noms d'acteurs.
 * @param   critere: les critere de recherche de l'utilisateur.
 * @retval  succes:     valeur booleen 1 = le nom recherche est dans la ligne.
 *          echec:      valeur booleen 0 = le nom recherche n'est pas dans la ligne.
 */
unsigned int comparer_donnee_nom_critere(
    h_donnee_nom    const donnee,
    pt_critere      const critere
);

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#endif //__COMPARAISON_H
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/