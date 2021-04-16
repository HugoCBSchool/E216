#ifndef __FILE_SYSTEM_H
#define __FILE_SYSTEM_H
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include <sys/file.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <poll.h>

#include "util/util.h"
#include "util/fstr.h"
#include "util/linklist.h"
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                                    T_DIR_NODE
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/**
 * @brief  Le t_dir_node encapsule la sychro et l'accès aux devices(fichiers) utilises par 
 *          le projet. Chaque node a un file descriptor correspondant a un des types 
 *          dir_node_type
 */ 
//========================================================================================
#define DIR_NODE_NEW_FACTORY_REG_NB_ELEMENT 6 //nombre de types au registre du factory
//----------------------------------------------------------------------------------------
/**
 * @brief  registre des concrétisations du type abstrait dir_node
 */
typedef enum dir_node_type{
    e_fifo=0,       /*un fifo*/
    e_channel=1,    /*dir contenant une paire de fifos nommés rx.fifo et tx.fifo*/
    e_dir=2,        /*un directory qui existe deja obligatoirement*/
    e_file=3,       /*un fichier qui doit exister avant l'appel au factory.*/
    e_virtual=4,    /*un fichier qui n'existe pas réellement, sert de point d'amare a 
                      d'autre nodes*/
    e_lockf=5       /*un fichier qui ne contient rien et sert a generaliser flock() a 
                      tous les types de node.*/
}t_dir_node_type;
//----------------------------------------------------------------------------------------
/**
 * @brief  la structure rendue publique du t_dir_node
 * @note   elle peut etre fabriquee avec son constructeur sans quoi elle est inutile
 */
typedef struct dir_node{
    str                 name;           //le nom du inode au filesystem
    int                 fd;             //le fd du inode lors du open() par le factory
    struct dir_node*    parent;         //le inode parent du dir_node
    h_list              childs;         //la liste de inodes enfants du dir_node
    t_dir_node_type     type;           //le type concret du inode
    bool                remove_at_free; // flag pour signaler a dir_node_free de unlink 
                                        //le inode
    struct dir_node*    lock;           //un inode temporaire pour avoir accès a flock sur 
                                        //tout type de fd
}t_dir_node,*h_dir_node;
//----------------------------------------------------------------------------------------

/**
 * @brief  Factory virtualisant la construction dun dirnode.
 * @note
 * @param  parent:  le node parent. (NULL si path absolu && type=e_dir ou si type=virtuel)
 * @param  name:    le nom du node au filesystem(ou pas si virtuel)
 * @param  type:    le type de node a construire
 * @param  oflags:  flags POSIX-O (O_RDWR,etc.)
 * @retval le dirnode sous succes ou null sous echec(errno est set si jamais)
 */
h_dir_node  dir_node_new(h_dir_node parent,cstr name,t_dir_node_type type,int oflags);

/**
 * @brief  Destructrice de dir_node.
 * @note   RECURSIVE, S'assurer que les fichiers/directory/fifo a remove() ont l'attribut 
 *          remove_at_free set a TRUE.
 * @param  node: le node a detruire
 * @retval None
 */
extern void dir_node_free(void* node);

/**
 * @brief  encapsule les fonctionalites de flock() generalisees aux processus et aux 
 *          threads. Se comporte comme flock(), sauf que deux threads du meme processus 
 *          appelant cette fonction, un pour LOCK_EX et l'autre pour LOCK_SH, se 
 *          comporteront comme s'ils etaient des processus differents a la place.  
 * @param  node: le node a lock
 * @param  perm: la permission de lock (LOCK_UN,LOCK_SH,LOCK_EX).C.F. flock(2) RTFM.
 * @param  oflags: les flags presentes ci bas ou 0 pour aucun.
 * @retval TRUE sous succes, FALSE sous erreur ou si OPTLOCK_TRY et lock deja bare.
 *          errno pour en savoir plus.
 */
bool dir_node_lock(h_dir_node node,int perm,int oflags);
//options de lock de la fonction
#define OPTLOCK_RECURSE 0b1     //permet de recursivement lock les node enfants
#define OPTLOCK_TRY     0b01    //permet de faire un call non bloquant
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                      FILE_SYSTEMS PUBLIC(CLIENT) ET PRIVE(SERVEUR)
//========================================================================================
/**
 * @brief  Les interfaces presentant les rootnode des filesystems au client de l'api
 */

//==============================================================
//                CONSTANTES ET MACROS PUBLIQUES
//==============================================================
#define SERVER_START O_NOFOLLOW
static cstr E216_SERVER_NAME="E216_SERVER";
//--------------------------------------------------------------
/**
 * @brief  Interface du array de channels des file_system
 */
#define NB_CHANNEL 16   //le nombre de channels autre que le channel 0
//-------------------------------------------
#ifndef __COMPILATION_CLIENT
    //dans le cas du serveur, on prent rx.fifo pour RXID et tx.fifo pour TXID
    #define RXID(x) x%NB_CHANNEL        
    #define TXID(x) RXID(x)+NB_CHANNEL
#else
    //dans le cas du client,on prend l'inverse de pour le serveur
    #define TXID(x) x%NB_CHANNEL
#   define RXID(x) RXID(x)+NB_CHANNEL
#endif
//==============================================================



//==============================================================
//       PUBLIC_FILE_SYSTEM
//==============================================================
/**
 * @brief  interface plate exposant les enfants du h_dir_node membre nommé root au user
 * @note   USAGE CLIENT.
 */
typedef struct public_file_system{
    h_dir_node root;                //le root du file_system publique
    h_dir_node channels_dir;        //le node du dir contenant les channel
    h_dir_node* channels;           //tous les channels(fifo) sous forme d'un array
}t_public_file_system,*h_public_file_system;
//-------------------------------------------------------------
/**
 * @brief   -Verifie que le singleton du serveur est present et pas barré dans /tmp/
 *          -Initialise les acces(open()) au filesystem (channels/fifo)
 * @retval NULL sous erreur(voir errno)
 */
h_public_file_system public_file_system_open(void);
/**
 * @brief  destructeur de filesystem publique
 * @note   partant du root, close() tout,unlink()/rmdir() les node marques de 
 *          l'attribut remove_at_free
 * @param  pub: le filesystem
 * @retval None
 */
void                 public_file_system_close(void* pub);
//==============================================================


//==============================================================
//       PUBLIC_FILE_SYSTEM
//==============================================================
/**
 * @brief  interface plate exposant les enfants du membre root a l'utilisateur
 * @note   USAGE SERVEUR
 */
typedef struct private_file_system{
    t_public_file_system public_fs;     //heritage de l'interface publique
    h_dir_node root;                    //le root du filesystem complet
    h_dir_node data_store;              //le dir contenant les fichiers de donnees
    h_dir_node db_files[3];             //les fichiers de data sous forme dun array
    h_dir_node server_node;             //le node du serveur
}t_private_file_system,*h_private_file_system;
//-------------------------------------------------------------
/**
 * @brief  - Initialise le filesystem 
 *         - verifie la presence des dependances externes comme 
 *            les db qui doivent etre dans l'environment var "IMDB_DATA_PATH"
 *         - Sassure d'etre singleton sur le profile
 *         - Cree les elements ephemeres et temporaires (caches/fifo)
 * @note   LE ROOT EST LOCK LORS DE SUCCES, IL FAUT LE DEBARER POUR QUE LES AUTRES PROCESS 
 *         PUISSENT COMPLETER LEUR APPEL A public_file_system_open()
 * @retval le file_system sous succes, NULL sur erreur, voir errno
 */
h_private_file_system   private_file_system_open(void);

/**
 * @brief  - Partant du root du filesystem, ferme tous ses enfants, en s'assurant de 
 *           supprimer les ephemeres/temporaires tout en respectant POSIX pour la 
 *           suppression de ressources partagees.
 * @note   RECURSIVE
 * @param  priv: le filesystem a supprimer
 * @retval None
 */
void                    private_file_system_close(void* priv);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%







