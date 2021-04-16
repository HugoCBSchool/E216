
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "util/fstr.h"




#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

struct s_shmutex{
    int                 shmem_fd;
    str                 name;
    int                 exists;
    pthread_mutex_t*    lock;
};
typedef struct s_shmutex t_shmutex,*h_shmutex;

h_shmutex shmutex_init(char *name) {

    h_shmutex mutex = NULL;

    if(!(mutex=malloc(sizeof(struct s_shmutex))))
        return NULL;

    errno = 0;

    //open the shared mem for the mutex
    mutex->shmem_fd = shm_open(name, O_RDWR, 0660);
    
    //if no such shared mem
    //reopen it to create it
    if(errno == ENOENT) {
        mutex->shmem_fd = shm_open(name, O_RDWR|O_CREAT, 0660);
        mutex->exists = 1;
    }
    
    //if the file descriptor is bad
    //return in failure
    if(mutex->shmem_fd == -1) {
        perror("shm_open");
        return NULL;
    }

    //adjust the shmem size to fit the mutex
    if (ftruncate(mutex->shmem_fd, sizeof(pthread_mutex_t)) != 0) {
        perror("ftruncate");
        return mutex;
    }

    // Map pthread mutex into the shared memory.
    void *addr = mmap(
        NULL,
        sizeof(pthread_mutex_t),
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        mutex->shmem_fd,
        0
    );
    if (addr == MAP_FAILED) {
        perror("mmap");
        return mutex;
    }
    pthread_mutex_t *mutex_ptr = (pthread_mutex_t *)addr;

    // If shared memory was just initialized -
    // initialize the mutex as well.
    if (mutex->created) {
        pthread_mutexattr_t attr;
        if (pthread_mutexattr_init(&attr)) {
          perror("pthread_mutexattr_init");
          return mutex;
        }
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) {
        perror("pthread_mutexattr_setpshared");
        return mutex;
        }
        if (pthread_mutex_init(mutex_ptr, &attr)) {
        perror("pthread_mutex_init");
        return mutex;
        }
    }
    mutex.ptr = mutex_ptr;
    mutex.name = (char *)malloc(NAME_MAX+1);
    strcpy(mutex.name, name);
    return mutex;
}

int shared_mutex_close(h_shmutex mutex) {
  if (munmap((void *)mutex.ptr, sizeof(pthread_mutex_t))) {
    perror("munmap");
    return -1;
  }
  mutex.ptr = NULL;
  if (close(mutex.shmem_fd)) {
    perror("close");
    return -1;
  }
  mutex.shmem_fd = 0;
  free(mutex.name);
  return 0;
}

int shared_mutex_destroy(h_shmutex mutex) {
  if ((errno = pthread_mutex_destroy(mutex.ptr))) {
    perror("pthread_mutex_destroy");
    return -1;
  }
  if (munmap((void *)mutex.ptr, sizeof(pthread_mutex_t))) {
    perror("munmap");
    return -1;
  }
  mutex.ptr = NULL;
  if (close(mutex.shmem_fd)) {
    perror("close");
    return -1;
  }
  mutex.shmem_fd = 0;
  if (shm_unlink(mutex.name)) {
    perror("shm_unlink");
    return -1;
  }
  free(mutex.name);
  return 0;
}

struct sh_file{
    str         file_path;
    int         nb_line;
    size_t      line_sz;

    char*       mmap_line_table;

    int         sh_fd;

    h_shmutex   mutex;
    
}

int f(int fd){
    FILE* f=fdopen(fd,"r");
    if(fd)
};

typedef short cache_index;

#define PAGE_SZ     4096
#define FILE_EXISTS 0

typedef cache_state{
    CACHE_MEM,
    CACHE_DISK,
    CACHE_INVALID,
    CACHE_ERROR
};

struct s_cache{
    uint            nb_line;
    uint            line_offset;
    str             path;
    str             cache_head;
    pthread_mutex_t lock;
};
typedef struct s_cache t_cache,*h_cache;

h_cache cache_new(cstr name){
    uint            cache_sz=nb_line*(init_line_len+1);
    h_cache    nouv    =NULL;


    //init checkup
    if( !name                      || (access(name,F_OK) == FILE_EXISTS)  || 
        (cache_sz > sizeof(off_t)) || !(nouv = malloc(sizeof(cache_sz)) )  )
        return NULL;

    cache_sz=(
        ( cache_sz % PAGE_SZ )==0
        ?   ( ( cache_sz - (cache_sz%PAGE_SZ) ) + PAGE_SZ )
        :   cache_sz
    ); 
};

bool  cache_load(h_cache cache);
bool  cache_save(h_cache cache);
bool  cache_set_line(h_cache cache,cstr line);
char* cache_get_line(h_cache cache,unsigned int line);

cache_store()

cache_new_from_file()