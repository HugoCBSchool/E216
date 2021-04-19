#include "linklist.h"


//==================================================================================

//----------------------------------------------------------------------------------
h_list_node list_node_new(){
    return (h_list_node)calloc(1,sizeof(t_list_node));
}
/*
void        list_node_free( h_list_node node, fp_dtor dtor, unsigned recurse ){
    if(!node)
        return;

    if(dtor){ 
        dtor(node->data);
        node->data=NULL;
    }
    if(recurse){
        list_node_free(node->next,dtor,recurse);
        if(node->prev) 
            node->prev->next=NULL;
    }else{
        if(node->next) 
            node->next->prev=node->prev;
        if(node->prev) 
            node->prev->next=node->next;
    }
    free(node);
    node=NULL;
}*/
void*       list_node_pop(  h_list_node node ){
    if(!node)
        return NULL;
    
    void* data=node->data;
    node->data=NULL;
    if(node->prev) node->prev->next=node->next;
    if(node->next) node->next->prev=node->prev;

    free(node);
    node=NULL;

    return data;
}
BOOL         list_node_swap( h_list_node lhs, h_list_node rhs ){
    if(!lhs||!rhs)
        return FALSE;

    h_list_node temp=lhs->data;
    lhs->data=rhs->data;
    rhs->data=temp;
    
    return TRUE;
}
//==================================================================================



//==================================================================================

//----------------------------------------------------------------------------------
h_list  list_new(  fp_dtor dtor, fp_comp comp ){
    h_list ret=(h_list)calloc(1,sizeof(struct list));
    if( !ret )
        return NULL;

    ret->sorted=TRUE;
    ret->comp=comp;
    ret->dtor=dtor;

    return ret;
}
void    list_free( h_list list ){
    if(!list)
        return;
    
    list->itor=NULL;
    list_clear(list);
    free(list);
    list=NULL;
}
//----------------------------------------------------------------------------------
BOOL     list_is_sorted( h_list list ){
    return list->sorted;
}
BOOL     list_sort(      h_list list ){
    if(!list)
        return FALSE;

    if( !list->head || !list->head->next ) 
        list->sorted=TRUE;

    if( list->sorted )                      
        return TRUE;


    //bubblesort
    for(    h_list_node ptr=NULL,tail=NULL; /*declaration of scope*/
            !list->sorted;                  /*while not sorted    */
            tail=ptr                        /*bring back the tail by 1 each iteration*/
    ){  

        list->sorted=TRUE;                  //suppose sorted

        for(    ptr=list->head;             /*start lookup at head*/
                ptr->next!=tail;            /*lookup until the tail of sort region*/
                ptr=ptr->next               /*goto next on each iteration*/
        ){
            //if unsorted pair found
            if(!list->comp(ptr->data,ptr->next->data)){
                //sort them by swaping data and remember we swapped
                list->sorted=FALSE;
                list_node_swap(ptr,ptr->next);
            }
        }
    }
    return TRUE;
}
//----------------------------------------------------------------------------------
BOOL     list_get_nb_elem(     h_list list ){
    return list->n_elem;
}
void*    list_get_elem_index(  h_list list, unsigned long index ){
    h_list_node node=NULL;
    if(!list || index>=list->n_elem || !list->head)
        return NULL;
    
    if(index<(list->n_elem)/2){
        node=list->head;
        for(int i=0;i!=index;++i){
            node=node->next;
        }
    }else{
        node=list->tail;
        for(int i=list->n_elem-1;i>index;--i){
            node=node->prev;
        }
    }
    return node->data;
}
BOOL     list_set_elem_index(  h_list list, unsigned long index, void* data ){
    h_list_node node=NULL;
    if( !list || !data || index>=list->n_elem || !list->head)
        return FALSE;
    
    if(index<(list->n_elem)/2){
        node=list->head;
        for(int i=0;i!=index;++i){
            node=node->next;
        }
    }else{
        node=list->tail;
        for(int i=list->n_elem-1;i>index;--i){
            node=node->prev;
        }
    }
    if(node->data){
        if(list->dtor)
            list->dtor(node->data);
        node->data=NULL;            
    }
    node->data=data;
    return TRUE;
}
BOOL     list_set_destructeur( h_list list, fp_dtor dtor ){
    if( !list || list->n_elem)
        return FALSE;
    list->dtor=dtor;
    return TRUE;
}
BOOL     list_set_comparateur( h_list list, fp_comp comp ){
    if( !list || list->n_elem)
        return FALSE;
    list->comp=comp;
    return TRUE;
}
//----------------------------------------------------------------------------------
void*   list_itor_head( h_list list ){
    if(!list)   return NULL;
    list->itor=list->head;
    return list->itor?list->itor->data:NULL;
}
void*   list_itor_tail( h_list list ){
    if(!list)   return NULL;
    list->itor=list->tail;
    return list->itor?list->itor->data:NULL;
}
void*   list_itor_next( h_list list ){
    if( list == NULL || list->itor == NULL )   
        return NULL;
    list->itor=list->itor->next;
    return list->itor?list->itor->data:NULL;
}
void*   list_itor_prev( h_list list ){
    if(!list||!list->itor)   return NULL;
    list->itor=list->itor->next;
    return list->itor?list->itor->data:NULL;
}
void    list_foreach_valeur_context( h_list list, fp_val_ctx f, void* context ){
    if(!( list && f && list->n_elem ))
        return;

    for(    h_list_node cur=list->head;
            cur;
            cur=cur->next
    ){
        f(cur->data,context);
    }
}
//----------------------------------------------------------------------------------
BOOL     list_push_index( h_list list, void* data, unsigned long index ){
    if( !( list && data && (index<=list->n_elem)))
        return FALSE;

    h_list_node nouv=list_node_new();
    if(nouv)
        return FALSE;
    
    nouv->data=data;

    if(!index){
        nouv->next=list->head;
        if(list->head)
            list->head->prev=nouv;
        list->head=nouv;
    }else{
        h_list_node dest=list->head;
        for(int i=1;i<index;++i){
            dest=dest->next;
        }
        nouv->prev=dest;
        nouv->next=dest->next;
        nouv->prev->next=nouv;
        if(nouv->next)
            nouv->next->prev=nouv;
    }
    ++list->n_elem;
    list->sorted=FALSE;
    list->tail=list->tail?list->tail:nouv;
    return TRUE;
}
BOOL     list_push_head(  h_list list, void* data ){

    if(!list||!data)
        return FALSE;

    h_list_node nouv=list_node_new();

    if(!nouv)
        return FALSE;

    nouv->next=list->head;
    nouv->data=data;

    if(nouv->next)
        nouv->next->prev=nouv;

    list->head=nouv;
    list->tail=list->tail?list->tail:nouv;
    ++list->n_elem;
    list->sorted=FALSE;

    return TRUE;
}
BOOL     list_push_tail(  h_list list, void* data ){
    if(!list||!data)
        return FALSE;
    
    h_list_node nouv=list_node_new();

    if(!nouv)
        return FALSE;
    
    nouv->data=data;

    nouv->prev=list->tail;
    if(nouv->prev)
        nouv->prev->next=nouv;

    list->tail=nouv;
    list->head=list->head?list->head:nouv;
    ++list->n_elem;
    list->sorted=FALSE;
     return TRUE;
}
BOOL     list_push_itor(  h_list list, void* data ){
    if(!list||!data||!list->itor)
        return FALSE;
    
    h_list_node nouv=list_node_new();
    
    if(!nouv)
        return FALSE;
    
    nouv->data=data;

    nouv->prev=list->itor;
    nouv->next=list->itor->next;
    list->itor->next=nouv;

    if(nouv->next)
        nouv->next->prev=nouv;

    ++list->n_elem;
    list->sorted=FALSE;
    list->head=list->head?list->head:nouv;
    list->tail=list->tail?list->tail:nouv;
    return FALSE;
}
BOOL     list_push_sort(  h_list list, void* data ){
    if(!list||!data||!list->comp)
        return FALSE;

    if(!list->n_elem){
        int ret     =FALSE;
        int sorted  =list->sorted;

        ret=list_push_head(list,data);
        list->sorted=sorted;

        return ret;
    }

    h_list_node nouv=list_node_new();
    h_list_node cur=list->head;

    if(!nouv)
        return FALSE;

    nouv->data=data;
    
    while( cur && list->comp(cur->data,data)){
        cur=cur->next;
    }

    nouv->prev=cur;
    nouv->next=cur->next;
    cur->next=nouv;
    if(nouv->next)
        nouv->next->prev=nouv;
    
    ++list->n_elem;
    list->head=list->head?list->head:nouv;
    list->tail=list->tail?list->tail:nouv;
    return TRUE;

}
//----------------------------------------------------------------------------------
void*   list_pop_index( h_list list, unsigned long index ){
    void* ret=NULL;
    h_list_node cur=NULL;

    if(!list||index>=list->n_elem)
        return NULL;
    
    
    if( list->n_elem==0 && (list->head || list->tail || list->itor ) ){
        printf("LIST_POP ERROR EMPTY LIST WITH REMAINING POINTERS\n");
        return NULL;
    }


    cur=list->head;
    for(int i=0;i<index;++i)    cur=cur->next;

    if(cur==list->tail)          list->tail=list->tail->prev;
    if(cur==list->head)          list->head=list->head->next;
    
    ret=list_node_pop(cur);


    --list->n_elem;
    return ret;
}
void*   list_pop_head(  h_list list ){
    h_list_node tmp=NULL;
    void* ret=NULL;

    if(!list||!(tmp=list->head))
        return NULL;



    if( list->n_elem==0 && (list->head || list->tail || list->itor ) ){
        printf("LIST_POP ERROR EMPTY LIST WITH REMAINING POINTERS\n");
        return NULL;
    }
    if(list->head==list->tail)list->tail=tmp->next;
    list->head=tmp->next;
    ret=list_node_pop(tmp);

    --list->n_elem;

    return ret;
}
void*   list_pop_tail(  h_list list ){
    h_list_node tmp=NULL;
    void* ret=NULL;

    if(!list||!(tmp=list->tail))
        return NULL;

    if( list->n_elem==0 && (list->head || list->tail || list->itor ) ){
        printf("LIST_POP ERROR EMPTY LIST WITH REMAINING POINTERS\n");
        return NULL;
    }
    if(list->head==list->tail)list->head=tmp->prev;
    list->tail=tmp->prev;
    ret=list_node_pop(tmp);

    --list->n_elem;
    

    return ret;
}
void    list_clear(h_list list){
    if(!list)
        return;

    for(void* data=list_pop_head(list);
        data;
        data=list_pop_head(list)
    ){
        if(list->dtor)
            list->dtor(data);
        data=NULL;
    }
    for(void* data=list_pop_tail(list);
        data;
        data=list_pop_tail(list)
    ){
        if(list->dtor)
            list->dtor(data);
        data=NULL;
    }
    list->head=NULL;
    list->tail=NULL;
    list->itor=NULL;
    
}
//==================================================================================
