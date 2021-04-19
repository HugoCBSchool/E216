/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*
    Projet      : ELE216 - Laboratoire 1
    Date        : 09 fevrier 2021
    Par         : Gabriel Gandubert et Hugo Cusson-Bouthillier
    Definition  : Manipulation des chaines de caracteres de ligne provenant des bases
                  de donnees du programme.

    Contient:
        FONCTIONS:
            - hmap_creer                : constructeur de la hmap.
            - hmap_remplacer            : remplace la paire pointee par la cle.
            - hmap_get                  : sort une reference de la structure stoquee
            - hmap_pop_element          : retire un element de la table et la retourne.
            - hmap_supprimer_element    : supprime un element de la table.
            - hmap_iterer_paires        : permet d'appeler une fonction pointee sur chaque
                                            element du hmap.
            - hmap_iterer_executer_si   : permet d'appeler une fonction pointee sur chaque
                                            element du hmap et une fonction de condition
                                            d'execution de la fonction exec.
            - hmap_get_taille           : sort la taille du hmap.
            - hmap_liberer              : libere le hmap.
*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#ifndef _HASHMAP_H
#define _HASHMAP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debug_include.h"
#include "funk.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							Definitions de types          								*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

//definition du type du hashmap et de son handle
typedef struct hmap     t_hmap      ,*h_hmap     ;
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/




/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							Debut des fonctions publiques								*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief  creation dun hashmap vide dont la taille initiale est taille_init*sizeof(void*)
 * @retval un pointeur sur le hashmap si tout va bien, NULL sinon
 */
h_hmap hmap_creer();


/**
 * @brief  permet d'ajouter une copie d'une paire clef-valeur au map sous la clef donnee
 * @param  map:             la map a populer
 * @param  cle:             la cle de l'element
 * @param  valeur:          laddresse de la structure abstraite aux yeux du map a copier
 *                          -   si 0, echoue
 * @param  f_liberer_valeur: un foncteur permettant de liberer la struct copiee
 *                          - si NULL, le map ne detruit pas le handle lorsquil est detrui
 * @retval EXIT_FAILURE si echec, EXIT_SUCCESS sinon
 *          - un echec arrive si la cle est deja populee dans la table
 */
int hmap_add(  h_hmap const         map     ,   
               const  char*  const  cle     , 
               void*                valeur  , 
               const  fp_dtor const f_liberer_valeur  );
/**
 * @brief  permet d'ajouter une copie d'une paire clef-valeur au map sous la clef donnee
 *          Si la cle existe deja, elle est detruite si un destructeur y est attribue
 *          ensuite, la reference de la nouvelle sert a effectuer le meme traitement 
 *          que pour hmap_add.
 * @param  map:             la map a populer
 * @param  cle:             la cle de l'element
 * @param  valeur:          laddresse de la structure abstraite aux yeux du map a copier
 * @param  f_liberer_valeur: un foncteur permettant de liberer la struct copiee
 *                              - si NULL, le map ne garde quune reference sur la struct
 * @retval le nombre de bytes copies, -1 pour un echec d'allocation
 */
int hmap_remplacer( h_hmap const          map     ,   
                    const  char*  const   cle     , 
                    void*                 valeur  ,   
                    const  fp_dtor const  f_liberer_valeur );

/**
 * @brief  permet d'obtenir une reference sur la structure stoquee.
 * @param  map: la map a observer
 * @param  cle: la cle a chercher
 * @retval un pointeur sur la structure dont le titre est inconnu de la map.
 *          NULL si echec.
 */
void *hmap_get( h_hmap const map, const char *const   cle );

/// @brief Remove an element from the hashmap.
/// @param hashmap The hashmap to remove from.
/// @param key The string key to use.
/// @param len The length of the string key.
/// @return On success 0 is returned.
/**
 * @brief  permet de retirer un element de la table sans liberer la memoire de la 
 *         struct stockee.
 * @param  map: la map a observer 
 * @param  cle: la cle a chercher
 * @retval un pointeur sur la struct retiree dont la responsabilite est remise a 
 *          l'appelant. NULL si echec.
 */
void* hmap_pop_element( h_hmap const map, const char *const cle);
/**
 * @brief  permet de retirer un element de la table et d'en liberer la memoire 
 *          si un foncteur de destruction est associe a l'element. Seulement utiliser 
 *          cette fonction si la gestion de memoire est faite a l'externe.
 * @param  map: la map a observer
 * @param  cle: la cle a chercher
 * @retval void
 */
void  hmap_supprimer_element( h_hmap const map, const char *const cle);


/**
 * @brief  permet d'iterer sur toutes les paires du map en appelant une procedure prenant 
 *          une paire cle:valeur en plus d'un contexte d'execution en argument.
 * @param  map:         la map sur laquelle iterer
 * @param  func:        l'addresse de la fonction a appeler
 * @param  contexte:    le contexte d'execution a donner en premier argument a la fonction
 * @retval 1 pour succes, 0 pour echec
 */
int hmap_iterer_paires( h_hmap const            map, 
                        fp_ctx_cle_val          func, 
                        void*                   contexte);
/**
 * @brief  permet d'iterer sur toutes les paires du map en appelant une f_si de format
 *             f_si_contexte_cle_valeur renvoyant  EXIT_SUCCESS/FAILURE. Si le resultat
 *             est SUCCESS, le map appelle alors f_exec de format fp_ctx_cle_val
 *             sur la paire.
 * @param  map:          la map sur laquelle iterer
 * @param  f_exec:       l'addresse de la fonction a appeler si f_si retourne SUCCESS
 * @param  contexte_exec:le contexte d'execution a donner en premier argumnt a la fonction
 * @param  f_si:         l'addresse de la fonction f_si
 * @param  contexte_si:  le contexte d'execution a donner en premier argumnt a f_si
 * @retval 1 pour succes, 0 pour echec
 */
int hmap_iterer_executer_si(    h_hmap const                map,
                                fp_ctx_cle_val              f_exec,
                                fp_ctx_cle_val_return_exit  f_si,
                                void* const                 contexte_exec,
                                void* const                 contexte_si);
/**
 * @brief  permet d'iterer sur toutes les paires du map en appelant une procedure 
 *          permettant de dicter au map si il doit supprimer ou pas la paire.
 *          si le retour est TRUE, il supprime.
 * @param  map:         la map sur laquelle iterer
 * @param  func:        l'addresse de la fonction a appeler
 * @param  contexte:    le contexte d'execution a donner en premier argument a la fonction
 * @retval EXIT_SUCCESS ou EXIT_FAILURE
 */
int hmap_iterer_paires_supprimer_si(   h_hmap const                map,
                                       fp_ctx_cle_val_return_exit  f_si,
                                       void* const                 contexte);

/**
 * @brief  retourne la taille du domaine de hashage de la map.
 * @param  map: la map a observer
 * @retval le nombre de pointeurs de paires chainees dans la table
 */
int64_t hmap_get_taille(h_hmap const map);


/**
 * @brief  permet de detruire un hmap. la memoire de chaque paire est liberee si celle-ci 
 *          contient un foncteur vers sa destructice.
 * @param  map: la map a detruire
 * @retval None
 */
void hmap_liberer(h_hmap map);

#endif