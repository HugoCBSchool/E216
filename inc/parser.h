/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*
	Projet		: ELE216 - Laboratoire 1
	Date		: 09 fevrier 2021
	Par 		: Gabriel Gandubert et Hugo Cusson-Bouthillier
	Definition	: Traitement des lignes de donnees des bases de donnees de films.

	Contient:

		FONCTIONS:
			% Division et comparaison d'une ligne du fichier des titres.
			h_donnee_titre parse_ligne_titre_et_critere(cstr const ligne, 
                                            pt_critere const  critere);
			% Division et comparaison d'une ligne du fichier des noms.
			h_donnee_nom   parse_ligne_nom_et_critere(  cstr const ligne, 
                                            pt_critere const  critere);
            % Division et comparaison d'une ligne du fichier des cotes.
            h_donnee_cote  parse_ligne_cote_et_critere( cstr const ligne, 
                                            pt_critere const  critere);
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef __PARSER_H
#define __PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/util.h"
#include "util/table_string.h"

#include "modules/comparaison.h"
#include "modules/critere.h"
#include "modules/resultat.h"


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							Debut des fonctions publiques								*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief  	Fonction permettant de subdiviser, et comparer une ligne de la base de donnees
 			des titres avec les criteres de l'utilisateur.
 * @note	HLR17: Parser de la base de donnee des titres de films.
 * @param   ligne   : la ligne a comparer de la base de donnees des titres de film.
 * @param   critere	: les criteres de selection a respecter (structure dans critere.h).
 * @retval	Elle retourne une structure encapsulant la ligne de la base de donnee en 
 			plusieurs elements. Si la ligne ne correspond pas a (aux) titre(s) donne(s)
 			ou qu'une erreur survient, elle retourne un pointeur nul.
 */
h_donnee_titre parse_ligne_titre_et_critere(cstr const       ligne       , 
                                            pt_critere const  critere           );

/**
 * @brief  	Fonction permettant de subdiviser, et comparer une ligne de la base de donnees
 			des noms avec les criteres de l'utilisateur.
 * @note	HLR16: Parser de la base de donnee des noms d'acteurs.
 * @param   ligne   : la ligne a comparer de la base de donnees des noms d'acteurs.
 * @param   critere	: les criteres de selection a respecter (structure dans critere.h).
 * @retval	Elle retourne une structure encapsulant la ligne de la base de donnee en 
 			plusieurs elements. Si la ligne ne correspond pas a (aux) acteur(s) donne(s)
 			ou qu'une erreur survient, elle retourne un pointeur nul.
 */
h_donnee_nom   parse_ligne_nom_et_critere(  cstr const       ligne       , 
                                            pt_critere const  critere           );

/**
 * @brief  	Fonction permettant de subdiviser, et comparer une ligne de la base de donnees
 			des cotes avec les criteres de l'utilisateur.
 * @note	HLR18: Parser de la base de donnee des cotes des films.
 * @param   ligne   : la ligne a comparer de la base de donnees des cotes d'acteurs.
 * @param   critere	: les criteres de selection a respecter (structure dans critere.h).
 * @retval	Elle retourne une structure encapsulant la ligne de la base de donnee en 
 			plusieurs elements. Si la ligne ne correspond pas a (aux) cote(s) donne(s)
 			ou qu'une erreur survient, elle retourne un pointeur nul.
 */
h_donnee_cote  parse_ligne_cote_et_critere( cstr const       ligne       , 
                                            pt_critere const  critere           );

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#endif //__PARSER_H
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/