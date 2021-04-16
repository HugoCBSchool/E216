#ifndef __SHARED_H
#define __SHARED_H

#include "util/util.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                               TYPEDEFS
//==========================================================================
/**
 * @brief  definition du type slock et son handle, une primitive de synchronisation 
 *         fort utile.
 * @note      Un slock est un shared_lock qui etend son pouvoir d'action au dela du thread 
 *          * boundary et audela du process boundary. il permet de garantir l'une de deux 
 *          * choses sur la ressources associee: les modes sont decris dans t_slock_perm.
 */
typedef struct s_slock t_slock, *h_slock;

/**
 * @brief  les differentes permissions pour un lock
 * @note   
 * @retval None
 */
typedef enum e_slock_perm{
    perm_shared,         /*equivalent a O_RDONLY, garantie l'integrite de la ressource*/
    perm_exclusive      /*  equivalent a O_RDWR,  garantie l'exclusivite de l'acces   */
} t_slock_perm;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


/**
 * @brief  fonction de creation de lock pour le processus proprietaire. cette fonction a 
 *         pour effet de creer un nouveau mmap dans lequel un nouveau mutex posix 
 *         est instancié.
 * @note   
 * @param  name: 
 * @retval 
 */
h_slock slock_create(cstr name);

/**
 * @brief  
 * @note   
 * @param  name: 
 * @retval 
 */
h_slock slock_open(cstr name);

/**
 * @brief  
 * @note   
 * @param  sh: 
 * @param  perm: 
 * @retval 
 */
bool    slock_trylock(h_slock sh,t_slock_perm perm);

/**
 * @brief  
 * @note   
 * @param  sh: 
 * @param  perm: 
 * @retval 
 */
bool    slock_lock(h_slock sh,t_slock_perm perm);

/**
 * @brief  
 * @note   
 * @param  sh: 
 * @retval 
 */
bool    slock_unlock(h_slock sh);
void slock_free(void* sh);

#endif