
#ifndef __LINKLIST_H
#define __LINKLIST_H



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "util.h"
#include "funk.h"
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief  
 * @note   
 * @retval None
 */
typedef struct list_node t_list_node,*h_list_node;
struct list_node{
    void*       data;
    h_list_node next;
    h_list_node prev;
};
typedef struct list t_list,*h_list;
struct list{
    unsigned long   n_elem;
    fp_comp         comp;
    fp_dtor         dtor;

    h_list_node     head;
    h_list_node     tail;
    h_list_node     itor;

    BOOL            sorted;
};


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief  
 * @note   
 * @param  dtor: 
 * @param  comp: 
 * @retval 
 */
h_list      list_new(fp_dtor dtor, fp_comp comp);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval None
 */
void        list_free(h_list list);
//----------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval 
 */
BOOL         list_is_sorted(h_list list);
/**
 * @brief  
 * @note   Serveur-HLR07: Tri de liste de numéros de ligne
 * @param  list: 
 * @retval 
 */
BOOL         list_sort(h_list list);
//----------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval 
 */
BOOL         list_get_nb_elem(h_list list);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @param  index: 
 * @retval None
 */
void*       list_get_elem_index(h_list list,unsigned long index);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @param  index: 
 * @param  data: 
 * @retval 
 */
BOOL         list_set_elem_index(h_list list,unsigned long index,void* data);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @param  dtor: 
 * @retval 
 */
BOOL         list_set_destructeur(h_list list,fp_dtor dtor);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @param  comp: 
 * @retval 
 */
BOOL         list_set_comparateur(h_list list,fp_comp comp);
//----------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval None
 */
void*       list_itor_head(h_list list);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval None
 */
void*       list_itor_tail(h_list list);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval None
 */
void*       list_itor_next(h_list list);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval None
 */
void*       list_itor_prev(h_list list);
//----------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @param  list: 
 * @param  index: 
 * @retval None
 */
void*       list_get_elem_index(h_list list,unsigned long index);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @param  index: 
 * @param  data: 
 * @retval 
 */
BOOL         list_set_elem_index(h_list list,unsigned long index,void* data);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @param  dtor: 
 * @retval 
 */
BOOL         list_set_destructeur(h_list list,fp_dtor dtor);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @param  comp: 
 * @retval 
 */
BOOL         list_set_comparateur(h_list list,fp_comp comp);
//----------------------------------------------------------------------------------
/**
 * @brief  
 * @note   :Serveur-HLR06 - mutateur de liste de numéro de ligne
 * @param  list: 
 * @param  data: 
 * @param  index: 
 * @retval 
 */
BOOL         list_push_index(h_list list,void* data,unsigned long index);
/**
 * @brief  
 * @note   :Serveur-HLR06 - mutateur de liste de numéro de ligne
 * @param  list: 
 * @param  data: 
 * @retval 
 */
BOOL         list_push_head(h_list list,void* data);
/**
 * @brief  
 * @note   :Serveur-HLR06 - mutateur de liste de numéro de ligne
 * @param  list: 
 * @param  data: 
 * @retval 
 */
BOOL         list_push_tail(h_list list,void* data);
/**
 * @brief  
 * @note   :Serveur-HLR06 - mutateur de liste de numéro de ligne
 * @param  list: 
 * @param  data: 
 * @retval 
 */
BOOL         list_push_itor(h_list list, void* data);
/**
 * @brief  
 * @note   :Serveur-HLR06 - mutateur de liste de numéro de ligne
 * @param  list: 
 * @param  data: 
 * @retval 
 */
BOOL         list_push_sort(h_list list, void* data);
//----------------------------------------------------------------------------------
/**
 * @brief  
 * @note   
 * @param  list: 
 * @param  index: 
 * @retval None
 */
void*       list_pop_index(h_list list,unsigned long index);
/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval None
 */
void*       list_pop_head(h_list list);

/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval None
 */
void*       list_pop_tail(h_list list);

/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval None
 */
void*       list_pop_itor(h_list list);

/**
 * @brief  
 * @note   
 * @param  list: 
 * @retval None
 */
void list_clear(h_list list);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#endif //__LINKLIST_H