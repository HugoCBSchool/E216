
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

#include "util/shared.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#define MAX_BUF_SZ 256

typedef enum lockstate{
    s_shared,
    s_excl,
    s_shared_flush,
    s_none
} t_lockstate;


struct s_slock{
    pthread_mutex_t     mutex;          //the mutex
    pthread_cond_t      state_change_event;
    t_lockstate        lock_state;

    int                 shared_count;   //the shared-permission owner count
    int                 shared_queue;
    int                 excl_queue;

    bool                in_use;         //the exclusive-permission ownership flag
    pid_t               pid_owner;      //the exclusive-permission owner pid
    pthread_t           thread;         //the exclusive-permission owner tid
};
t_lockstate next_state(h_slock lock){
    switch(lock->lock_state){
        case s_excl:{
            lock->in_use=FALSE;
            if(lock->excl_queue>0){
                return s_excl;
            }else{
                return s_shared;
            }
        }
        case s_shared:{
            if(lock->excl_queue>0){
                if(lock->shared_count>0){
                    return s_shared_flush;
                }else{
                    return s_shared;
                }
            }else{
                return s_shared;
            }
        }
        case s_shared_flush:{
            if(lock->shared_count>0){
                return s_shared_flush;
            }else{
                return s_excl;
            }
        }
        case s_none:{

        }
        default:{
            return s_shared;
        }
    }
};

h_slock slock_create(cstr name,int excl_buf_sz){
    int                 fd;
    h_slock             lock;
    pthread_mutexattr_t shmattr;
    pthread_condattr_t  shcattr_event_excl_close,shcattr_event_shared_close;

    if( (fd=open(name, O_RDWR | O_CREAT | O_EXCL,0666))<=0 ){
        printf("errno:%d    err:%s\n",errno,strerror(errno));
        return NULL;
    }

    ftruncate(fd,sizeof(struct s_slock));
    
    pthread_mutexattr_init(&shmattr);
    pthread_mutexattr_setpshared(&shmattr,PTHREAD_PROCESS_SHARED);
      
    //pthread_condattr_init(&shcattr_event_excl_close);
    //pthread_condattr_setpshared(&shcattr_event_excl_close,PTHREAD_PROCESS_SHARED);

    pthread_condattr_init(&shcattr_event_shared_close);
    pthread_condattr_setpshared(&shcattr_event_shared_close,PTHREAD_PROCESS_SHARED);

    lock=mmap(NULL,sizeof(struct s_slock),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    close(fd);


    pthread_mutex_init(&lock->mutex,&shmattr);
    //pthread_cond_init(&lock->state_change_event,&shcattr_event_excl_close);
    pthread_cond_init(&lock->state_change_event,&shcattr_event_shared_close);
    lock->shared_count=0;
    lock->pid_owner=0;
    lock->thread=0;
    lock->in_use=FALSE;
    lock->shared_queue=0;
    lock->excl_queue=0;
    lock->lock_state=s_shared;

    return lock;

}
h_slock slock_open(cstr name){
    int fd;
    h_slock lock=NULL;

    if((fd=open(name,O_RDWR,0666))<=0)
        return NULL;
    
    lock=mmap(NULL,sizeof(struct s_slock),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    close(fd);
    return lock;
}
bool slock_trylock(h_slock sh,t_slock_perm perm){
    
    if(!sh) return FALSE;

    switch(perm){
        case perm_shared:{
                //on acquire, retourne en erreur si echec car a echoue
                if( pthread_mutex_trylock(&sh->mutex)!=0 )
                    return FALSE;
                
                //pour assurer l'ordonancement, il faut que les queues soient vides
                //et que aucun excl occupe la region
                if(sh->lock_state!=s_shared){
                    pthread_mutex_unlock(&sh->mutex);
                    return FALSE;
                }
                //sinon incref le nombre de lock en shared permission                
                sh->shared_count++;

                //unlock
                pthread_mutex_unlock(&sh->mutex);

            }break;
        case perm_exclusive:{
                //si on a encore des shared-permission on echoue
                if(sh->shared_count!=0)
                    return FALSE;

                //on acquire, retourne en erreur si echec car a echoue
                if( pthread_mutex_trylock(&sh->mutex)!=0 )
                    return FALSE;
                
                //pour assurer de l'ordonancement, ilfaut que les queues soient vides
                //il faut aussi que aucun reader occupe la region
                if(sh->lock_state!=s_excl||sh->in_use==TRUE||sh->shared_queue>0){
                    pthread_mutex_unlock(&sh->mutex);
                    return FALSE;
                }

                //on a reussi
                //on senregistre comme owner    
                sh->pid_owner=getpid();
                sh->thread=pthread_self();

                //on set le lockgard
                sh->in_use=TRUE;

                //on ferme le mutex
                pthread_mutex_unlock(&sh->mutex);
                
                return TRUE;
            }break;
        default:
            return FALSE;
    }
    return TRUE;
}
bool slock_lock(h_slock sh,t_slock_perm perm){
        
    if(!sh) return FALSE;

    switch(perm){
        case perm_shared:{
                //on acquire, retourne en erreur si echec car a echoue
                if( pthread_mutex_lock(&sh->mutex)!=0 )
                    return FALSE;
                
                //incref la shared queue
                ++sh->shared_queue;

                //tant que des writer attendent
                while(sh->lock_state!=s_shared){
                    //on attend la prochain event de fermeture dun excl pour reverifier
                    if(pthread_cond_wait(&sh->state_change_event,&sh->mutex)!=0){
                        pthread_mutex_unlock(&sh->mutex);
                        return FALSE;
                    }
                }

                //decref la shared queuechr
                --sh->shared_queue;

                //incref le nombre de lock en shared permission
                sh->shared_count++;


                //unlock
                pthread_mutex_unlock(&sh->mutex);

            }break;
        case perm_exclusive:{
                //on acquire, retourne en erreur si echec car a echoue
                if( pthread_mutex_lock(&sh->mutex)!=0 )
                    return FALSE;
                
                //on incref la excl queue
                ++sh->excl_queue;

                //tant que un excl est en cours
                while(sh->in_use==TRUE){
                    //on attend la prochain event de fermeture dun excl pour reverifier
                    if(pthread_cond_wait(&sh->state_change_event,&sh->mutex)!=0){
                        pthread_mutex_unlock(&sh->mutex);
                        return FALSE;
                    }
                }

                //tant que des lecteurs sont encore actifs
                while(sh->lock_state!=s_excl){
                    //on attend le prochain evenement de fermeture dun lecteur pour reverifier
                    if(pthread_cond_wait(&sh->state_change_event,&sh->mutex)!=0){
                        pthread_mutex_unlock(&sh->mutex);
                        return FALSE;
                    }
                }

                //on decref la queue
                --sh->excl_queue;

                //on senregistre comme owner
                sh->pid_owner=getpid();
                sh->thread=pthread_self();

                //on set le write lockguard
                sh->in_use=TRUE;

                //on libere le mutex pour permettre au prochain thread de se mettre en queue
                pthread_mutex_unlock(&sh->mutex);

            }break;
        default:
            return FALSE;
    }
    return TRUE;
}
bool slock_unlock(h_slock sh){

    if(!sh) return FALSE;
    
    //prend le lock
    if(pthread_mutex_lock(&sh->mutex)!=0)
        return FALSE;

    //on verifie si on est dans un excl
    if(sh->in_use==TRUE){
        //si oui on confirme quon est le owner
        if(sh->pid_owner==getpid() && sh->thread==pthread_self()){
            //avant de release
            sh->in_use=FALSE;
            sh->pid_owner=0;
            sh->thread=0;
            sh->lock_state=next_state(sh);

            //on issue un signal pour autoriser le prochain excl en queue
            //ou pour debloquer les shared en queue
            pthread_cond_broadcast(&sh->state_change_event);

            pthread_mutex_unlock(&sh->mutex);

            return TRUE;
        }
        //sinon on release et exit eb erreur
        pthread_mutex_unlock(&sh->mutex);
        return FALSE;
    }
    
    //si on ne lest pas decref et signale
    --sh->shared_count;
    sh->lock_state=next_state(sh);

    //pour dire aux autres writers dans la queue de transiger l'execution suivante
    pthread_cond_broadcast(&sh->state_change_event);

    pthread_mutex_unlock(&sh->mutex);
    return TRUE;
}
void slock_free(void* sh){

    h_slock s=(h_slock)sh;

    if(!sh) return;

    munmap(s,sizeof(struct s_slock)); 

}