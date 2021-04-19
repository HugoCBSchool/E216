#ifndef __DEBUG_INCLUDE_H
#define __DEBUG_INCLUDE_H
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                          contexte de compile time                                */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * Simplement commenter les #define que lon ne veut pas compiler
 */

//#define __DEBUG						// active les main de debug

#include "base.h"

#ifdef  __DEBUG
//	#define __DEBUG_PHASE_1					// active le main de debug de la phase 1
//	#define __DEBUG_PARSER_H				// active le main de debug du parser.h
	#define __DEBUG_INTERFACE_H				// active le main de debug de interface.h
//  #define __INTERFACE_H_DEBUG_LECTURE		//active le debug de lecture fichier
    #define __INTERFACE_H_DEBUG_RESULTAT	//active le debug de recherche fichier
//	#define __DEBUG_HASHMAP			  		// active les feature de debug de hashmap

	#define __DEBUG_ANALYSE_RESULTAT //active les features de debug de analyser_resultat

#endif

//pour les utilitaires de debug avec print
#ifdef __DEBUG
#define DBG_PIPE stdout
#else
#define DBG_PIPE stderr
#endif


#ifdef __DEBUG_ANALYSE_RESULTAT
	//activent des traces console
	#define __TRACE_ANALYSE_RESULTATS
	#define __TRACE_VERIFIER_RESULTAT	// interface.h  verifier_si_resultat_valide
	#define __TRACE_HMAP_EXECUTER_SI	// hashmap.h	hmap_iterer_si_exec
	#define __TRACE_AFFICHER_RESULTAT	// resultat.h	donnee_afficher_resultat
#endif


#endif