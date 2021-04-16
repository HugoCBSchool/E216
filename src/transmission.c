#include "modules/transmission.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
struct channel{
    int         rx_fifo;
    int         tx_fifo;
    str         rx_fifo_name;
    str         tx_fifo_name;
    t_client_id channel_id;
};
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
h_packet packet_creer(t_client_id cx_id,t_packet_id pk_id,byte* data,uint len){
    h_packet pk=NULL;
    if(!len || !data || !( pk=(h_packet)malloc(sizeof(t_packet)) ) )
        return;
    
    memcpy( (pk->data) , data , (len<PACKET_DATA_LENGTH)?len:PACKET_DATA_LENGTH );
    if(len<PACKET_DATA_LENGTH)
        pk->data[len]=0;
    pk->tx_header.client_id=cx_id;
    pk->tx_header.packet_id=pk_id;
    
    return pk;    
}
void packet_liberer(void* pk){
    free(pk);
}
unsigned int packet_id_leq(void* lhs, void* rhs){
    return (
        lhs&&rhs
        ?(  (   (((h_packet)lhs)->tx_header.packet_id)    
                <=
                (((h_packet)lhs)->tx_header.packet_id)    )
            ?   (unsigned int)TRUE
            :   (unsigned int)FALSE                       )
        :   FALSE
    );
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
h_tx_msg tx_msg_creer_tx(t_client_id id, cstr buffer){
    byte*           b =(byte*)buffer;
    t_packet_id     i =0;
    h_tx_msg  tx=NULL;
    h_list          ls=NULL;

    if(    !buffer 
        || !( tx=(h_tx_msg)malloc(sizeof(t_tx_msg)) )  
        || !( ls=list_new(free,packet_id_leq)                   )
    ){
        free(tx);
        return NULL;
    }

    for(;
        strlen((char*)b)*sizeof(char)+1 > PACKET_DATA_LENGTH;
        b+=PACKET_DATA_LENGTH,++i
    ){
        if(!list_push_sort(ls,packet_creer(id,i,b,PACKET_DATA_LENGTH))){
            list_free(ls);
            free(tx);
            return NULL;
        }
    }
    if(!list_push_sort(ls,packet_creer(id,++i,b,strlen((char*)b)*sizeof(char)+1))){
        list_free(ls);
        free(tx);
        return NULL;
    }

    tx->transmission_header.client_id=id;
    tx->transmission_header.packet_id=list_get_nb_elem(ls);
    tx->liste_packets=ls;

    return tx;
}
h_tx_msg tx_msg_creer_rx(h_packet    packet_0){
    h_tx_msg msg =NULL;
    h_list   ls  =NULL;
    if(!packet_0||!(msg=malloc(sizeof(t_tx_msg)))||!(ls=list_new(free,packet_id_leq))){
        free(msg);
        return NULL;
    }
    
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
