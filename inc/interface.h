/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* 	Projet		:	ELE216 - Laboratoire 1 
	Date		:	21 janvier 2021 
	Par 		:	Gabriel Gandubert et Hugo Cusson-Bouthillier 
	Definition	:	Encapsulation des criteres
					recu par le programme principal

	Contient:
		FONCTION:
			int recherche_fichier(				--Recherche de criteres dans un fichier
				char* nom_fichier, 						
				pt_critere critere
			)
			analyser_resultats					--analyser les resultats
			imprimer_resultat_nom				--imprimer les resultats dans un fichier
													
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef __INTERFACE_H
#define __INTERFACE_H
#define __GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "modules/critere.h"
#include "modules/parser.h"
#include "util/hashmap.h"
#include "util/util.h"
#include "modules/file_system.h"
#include "debug_include.h"
#include "base.h"
#include "buffer.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							CONSTANTES D'IMPLEMENTATION     							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* 
 * @note	HLR15 : Ouverture possible uniquement pour certains fichiers specifiques.
 */
// Constantes des noms des fichiers de reference.


static const char FICHIER_TITRE[]	= "title_basics.tsv";
static const char FICHIER_NOMS[]	= "name_basics.tsv";
static const char FICHIER_COTE[]	= "title_ratings.tsv";

typedef struct resultats_recherche{
	h_hmap map_nom;
	h_hmap map_cote;
	h_hmap map_titre;
	h_hmap map_union;
}t_resultats_recherche;


#ifdef __DEBUG
static const char PATH_FICHIER_TITRE[]	= "./data/bd-films-imdb/title_basics.tsv";
static const char PATH_FICHIER_NOMS[]	= "./data/bd-films-imdb/name_basics.tsv";
static const char PATH_FICHIER_COTE[]	= "./data/bd-films-imdb/title_ratings.tsv";
static const char FICHIER_SORTIE_NOM[]			="resultat_nom.txt";
static const char FICHIER_SORTIE_TITRE[]		="resultat_titre.txt";
static const char FICHIER_SORTIE_COTE[]			="resultat_cote.txt";
static const char FICHIER_SORTIE_RESULTATS[]	="resultats.txt";
#endif

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							Debut des fonctions publiques								*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/**
 * @brief 	Fonctions d'operation sur les db.
 *   
 * @param  	pfs		 le filesystem du serveur
 * @param  	critere: les criteres a trouver dans le fichier texte.
 * 
 */
//retourne le resultat de recherche ou NULL si erreur
t_resultats_recherche* recherche_simult(fileFD dbroot,pt_critere crit);
//retourne 0 pour succes, -1 pour echec
int op_supprimer(fileFD dbroot,pt_critere crit);
//retourne 0 pour succes, -1 pour echec
int op_ajouter(h_private_file_system pfs, pt_critere crit);

/**
 * @brief	Permet de remplir le fichier de sortie des resultats des noms.
 * @note	HLR31: Visualisation des resultats dans un fichier texte pour tous les
 			resultats de recherche.
 * @retval	EXIT_SUCCESS 	si le fichier de resultat des noms est cree correctement.
 * 			EXIT_FAILURE	sinon.
 */
int imprimer_resultat_nom(cstr nom_fichier,h_hmap nom);
/**
 * @brief	Permet de remplir le fichier de sortie des resultats des cotes.
 * @note	HLR31: Visualisation des resultats dans un fichier texte pour tous les
 			resultats de recherche.
 * @retval	EXIT_SUCCESS 	si le fichier de resultat des cotes est cree correctement.
 * 			EXIT_FAILURE	sinon.
 */
int imprimer_resultat_cote(cstr nom_fichier,h_hmap cote);
/**
 * @brief	Permet de remplir le fichier de sortie des resultats des titres.
 * @note	HLR31: Visualisation des resultats dans un fichier texte pour tous les
 			resultats de recherche.
 * @retval	EXIT_SUCCESS 	si le fichier de resultat des titres est cree correctement.
 * 			EXIT_FAILURE	sinon.
 */
int imprimer_resultat_titre(cstr nom_fichier,h_hmap titre);
/**
 * @brief	Permet de remplir le fichier de sortie des resultats finaux.
 * @note	HLR31: Visualisation des resultats dans un fichier texte pour tous les
 			resultats de recherche.
 * @retval	EXIT_SUCCESS 	si le fichier des resultats finaux est cree correctement.
 * 			EXIT_FAILURE	sinon.
 */
int imprimer_resultat_final(cstr nom_fichier,h_hmap resultat);
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#endif
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
