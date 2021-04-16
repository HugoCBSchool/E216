#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <limits.h>     //for page size and such

#include "util/util.h"
#include "util/fstr.h"
#include "util/linklist.h"

#define RX_PIPE        0
#define TX_PIPE        1
#define TX_PIPE_FULL  -1
#define RX_PIPE_EMPTY -1
#define CHANNEL_0_UNAVAILABLE -1



typedef unsigned char   byte;
typedef pid_t           t_client_id;
typedef unsigned long   t_packet_id;



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
typedef struct channel t_channel,*h_channel;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
typedef struct transmission_header t_tx_header, *h_tx_header;
struct transmission_header{
    t_client_id client_id;
    t_packet_id packet_id;
};
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
const int PACKET_DATA_LENGTH = 512-sizeof(t_tx_header);
typedef struct packet t_packet,*h_packet;
struct packet{
    t_tx_header tx_header;
    byte        data[PACKET_DATA_LENGTH];
};

h_packet packet_creer(t_client_id cx_id,t_packet_id pk_id,byte* data,uint len);
void packet_liberer(void* pk);
unsigned int packet_id_leq(void* lhs, void* rhs);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
typedef struct transmission t_tx_msg, *h_tx_msg;
struct transmission{
    t_tx_header tx_header;
    h_list      liste_packet;
};
h_tx_msg tx_msg_creer_tx(t_client_id id, cstr buffer);
h_tx_msg tx_msg_creer_rx(h_packet packet_0);
h_packet tx_msg_get_packet_0(h_tx_msg msg);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
