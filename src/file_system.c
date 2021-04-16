

#include "modules/file_system.h"
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                                   constantes 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
static const char FICHIER_TITRE[]	= "title_basics.tsv";
static const char FICHIER_NOMS[]	= "name_basics.tsv";
static const char FICHIER_COTE[]	= "title_ratings.tsv";
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                          DIR_NODE(FACTORY)
//========================================================================================
//virtual method handle
typedef h_dir_node (*fp_dir_node_new)(h_dir_node,cstr,int);
//----------------------------------------------------------------------------------------
/**
 * @brief  Les methodes ci dessous sont celles encapsulees par dir_node_new()
 * @param  parent:  voir dir_node_new()
 * @param  name:    voir dir_node_new()
 * @param  type:    voir dir_node_new()
 * @retval voir dir_node_new()
 */
//default constructor
static h_dir_node  dir_node_new_default(    h_dir_node      parent, cstr name   ,
                                            t_dir_node_type type                );
//fifo constructor
static h_dir_node  dir_node_new_fifo(       h_dir_node  parent, cstr name   ,int oflags);
//channel constructor
static h_dir_node  dir_node_new_channel(    h_dir_node  parent, cstr name   ,int oflags);
//directory constructor
static h_dir_node  dir_node_new_dir(        h_dir_node  parent, cstr name   ,int oflags);
//file constructor
static h_dir_node  dir_node_new_file(       h_dir_node  parent, cstr name   ,int oflags);
//virtual anchor node constructor
static h_dir_node  dir_node_new_virtual(    h_dir_node  parent, cstr name   ,int oflags);
//lock file constructor
static h_dir_node  dir_node_new_lock(       h_dir_node  parent, cstr name   ,int oflags);
//virtual method lookup-table
static const fp_dir_node_new 
dir_new_concrete_ctor_register[DIR_NODE_NEW_FACTORY_REG_NB_ELEMENT]={
    dir_node_new_fifo,
    dir_node_new_channel,
    dir_node_new_dir,
    dir_node_new_file,
    dir_node_new_virtual,
    dir_node_new_lock
};
//======================================================================================
//                                    Publiques
//--------------------------------------------------------------------------------------
extern void dir_node_free(void* node){
    h_dir_node nd=node;
    int         lockfd;

    if(!node)
        return;
    
    if( (nd->lock&&nd->lock->fd!=-1) || nd->fd!=-1 )
        dir_node_lock(nd,LOCK_EX,0);

    
    if(nd->parent){
        int i=0;
        for(    h_dir_node n=list_itor_head(nd->parent->childs);
                (n!=nd)&&n;
                n=list_itor_next(nd->parent->childs)
        ){
            ++i;
        }

        list_pop_index(nd->parent->childs,i);

    }

    if(nd->remove_at_free){
        for(    h_dir_node n=list_itor_head(nd->childs);
                (n!=nd)&&n;
                n=list_itor_next(nd->childs)
        ){
            n->remove_at_free=TRUE;
        }
    }

    list_free(nd->childs);
    nd->childs=NULL;

    lockfd= ( (nd->lock)? nd->lock->fd : nd->fd);


    switch(nd->type){
        case e_virtual: break;
        case e_lockf:{
                h_dir_node temp=(
                    nd->parent 
                    ? ( nd->parent->type==e_fifo    ? 
                        nd->parent->parent          : 
                        nd->parent                  ) 
                    : NULL
                );
                perrno_if_not_true(
                    temp && (unlinkat(temp->fd,nd->name,0)==0),
                    "dir_node->free->unlinkat { parent : %s }{ self : %s }\n",
                    temp? temp->name: "none",
                    nd->name
                );
            }break;
        case e_file:
        case e_dir:
        case e_fifo:
        default:{
                dir_node_free(nd->lock);
                nd->lock=NULL;

                if(nd->remove_at_free){
                    char buf[256];  memset(buf,0,256*sizeof(char));
                    str nm=strdup("/proc/self/fd/%d                 ");
                    
                    sprintf(nm,"/proc/self/fd/%d",nd->fd);
                    perrno_if_not_true(
                        (readlink(nm,buf,256*sizeof(char)))==0,
                        "dir_node->free->readlink { parent : %s , self : %s }  \n\t link:%s \n\t path:%s\n",
                        nd->parent? nd->parent->name: "none",
                        nd->name,nm,buf
                    );

                    perrno_if_not_true(
                        remove(buf)==0,
                        "dir_node->free->remove { parent : %s }{ self : %s } path:%s\n",
                        nd->parent? nd->parent->name: "none",
                        nd->name,buf
                    );

                    free(nm);
                }
                
            }break;
    }
    if(nd->fd>=0)
        perrno_if_not_true(
            close(nd->fd)==0,
            "dir_node->free->close { parent : %s }{ self : %s }\n",
            nd->parent ? nd->parent->name : "none",
            nd->name
        );
    nd->fd=-1;
      
    str         tmp     =strdup(nd->name);
    h_dir_node  parent  =nd->parent,
                lock    =nd->lock;
    
    if(nd->name)
        free(nd->name);
    nd->name=NULL;
    
    free(nd);
    nd=NULL;

    perrno_if_not_true(
        flock(lockfd,LOCK_UN)==0,
        "dir_node->free->unlock { parent : %s }{ self : %s }\n",
        parent?parent->name:"none",
        tmp
    );

    if(lock)
        dir_node_free(lock);
    lock=NULL;


    free(tmp);
    tmp=NULL;
};
//-------------------------------------------------
bool dir_node_lock(h_dir_node node,int perm,int oflags){
    bool success=TRUE;
    h_dir_node lock=NULL;

    if(!node||(node->fd)==-1){
        return FALSE;
    }
    lock=(node->lock)?(node->lock):node;


    if( !(success&=(
        0==flock( lock->fd, perm|( (oflags&OPTLOCK_TRY)?LOCK_NB:0))) 
    )){
        perrno(
            "dir_node->lock->flock { parent : %s }{ self : %s }\n",
            node->parent?node->parent->name:"none",
            node->name
        );
        return FALSE;
    }

    if(OPTLOCK_RECURSE&oflags){
        int permission=perm;
        for(h_dir_node n = list_itor_head(node->childs);
            n;
            n = list_itor_next(node->childs)
        ){
            if(!(success&=dir_node_lock(n,perm,oflags))){
                perm=LOCK_UN;
                n=list_itor_head(node->childs);
            }
        }
        if(!success){
            perrno_if_not_true(
                (success&=(
                    flock( node->lock->fd, LOCK_UN|( (oflags&OPTLOCK_TRY) ? LOCK_NB : 0 ))
                )),
                "dir_node->lock->flock unlock { parent : %s }{ self : %s }\n",
                node->parent?node->parent->name:"none",
                node->name
            );
        }
    }

    return success;
}
//-------------------------------------------------
h_dir_node  dir_node_new(h_dir_node parent,cstr name,t_dir_node_type type,int oflags){
    h_dir_node nouv=NULL;
    str buf =NULL;
    int flags=oflags;
    int server_start=oflags&SERVER_START;
    oflags= oflags&(O_NOCTTY) ? 0 : oflags&~SERVER_START;
    
    if((!parent&& type!=e_virtual)|| (parent &&parent->fd==-1&&parent->type!=e_virtual))
        return NULL;

    switch(type){
        case e_lockf:   break;//disabled
        case e_file:{
            if(!(nouv=dir_new_concrete_ctor_register[type](parent,name,oflags))) 
                break;
            
            nouv->lock=NULL;
        }break;

        default:{

            if(!(nouv=dir_new_concrete_ctor_register[type](parent,name,oflags)))
                break; 
            
            if(!(dir_node_new_lock(
                    nouv,
                    name,
                    oflags|server_start
            ))){

                free(buf);
                buf=NULL;               
                
                dir_node_free(nouv);
                nouv=NULL;

                return NULL;
            }
            
            free(buf);
            buf=NULL;

            if( (nouv->parent) && parent->childs ){
                if(!list_push_tail(parent->childs,nouv)){
                    perrno(
                        "dir_node->new_default->push { parent : %s ,parent.nb_childs:%d}{ self : %s }\n",
                        nouv->parent ? nouv->parent->name : "none" ,
                        nouv->parent ? list_get_nb_elem(nouv->parent->childs):0,
                        nouv->name
                    );
                    dir_node_free(nouv);
                    nouv=NULL;

                    errno=ENOENT;
                    return NULL;
                }
            }
        }
    }
    //si on est dans le client et que l'on veut creer notre root
    if(!(oflags&O_CREAT) && nouv && nouv->type==e_virtual && !(flags&O_NOCTTY) ){
        
        //on verifie que le serveur est dispo
        h_dir_node tmp=dir_node_new(NULL,E216_SERVER_NAME,e_virtual,O_NOCTTY);
        if(!    dir_node_lock(
            tmp,
            LOCK_SH,
            oflags&O_NONBLOCK
            ?   OPTLOCK_TRY
            :   0
        )){
            //si on voulait nonblock ou que le lock a echoue
            //le serveur est non dispo ou le lock a echoue
            dir_node_free(nouv);
            dir_node_free(tmp);
            errno=ENOENT;
            return NULL;
        }
    }
    return nouv;
}
//======================================================================================
//                                    Privees
//--------------------------------------------------------------------------------------
static h_dir_node  dir_node_new_default(    h_dir_node parent, cstr name,
                                            t_dir_node_type type        ){
    h_dir_node nd=NULL;

    if(!(nd=calloc(1,sizeof(t_dir_node)))){
        perrno(
            "dir_node->new_default->calloc { parent : %s }{ self : %s }\n",
            nd->parent ? nd->parent->name : "none" ,
            nd->name
        );
        errno=ENOMEM;
        return NULL;
    }
    if(!(nd->name=strdup(name))){
        free(nd);
        nd=NULL;

        perrno(
            "dir_node->new_default->strdup { parent : %s }{ self : %s }\n",
            name ? nd->parent->name : "none" ,
            nd->name
        );
        errno=ENOMEM;

        return NULL;
    }

    nd->parent=parent;
    
    if(!(nd->childs=list_new(dir_node_free,NULL))){
        perrno(
            "dir_node->new_default->list_new { parent : %s }{ self : %s }\n",
            nd->parent ? nd->parent->name : "none" ,
            nd->name
        );
        if(parent){
            int tmp=list_get_nb_elem(parent->childs);
            perrno_if_not_true(
                (   list_pop_tail(parent->childs)           != NULL && 
                    (tmp-list_get_nb_elem(parent->childs))  == 1    ),
                "dir_node->new_default->pop { parent : %s ,nb_elem:(%d->%d) , self : %s }\n",
                nd->parent ? nd->parent->name : "none" ,
                tmp, list_get_nb_elem(parent->childs),
                nd->name
            );
        }

        dir_node_free(nd);
        nd=NULL;

        errno=ENOMEM;

        return NULL;
    }

    nd->fd=-1;
    nd->type=type;

    return nd;
}
static h_dir_node  dir_node_new_fifo(       h_dir_node parent, cstr name,int oflags){
    
    h_dir_node  fifo=NULL;
    str         buf=NULL;

    if( !(parent)   || !(parent->childs)|| 
        !name       || (parent->fd==-1)  || 
        (parent->type)!=e_channel       ){

        perrno(
            "dir_node->new_fifo->args { parent:%s , self : %s }\n",
            parent ? parent->name : "none" ,name
        );

        errno=EINVAL;
        return NULL;
    }

    if(!(fifo=dir_node_new_default(parent,name,e_fifo)))
        return NULL;
    
    //if in create mode
    if(oflags&O_CREAT){
        //create the fifo
        if(0!=mkfifoat(parent->fd,name,0666)){
            perrno(
                "dir_node->new_fifo->mkfifoat { parent:%s , self : %s }\n",
                parent ? parent->name : "none" ,
                name
            );
            
            dir_node_free(fifo);

            return NULL;
        }
        fifo->remove_at_free=TRUE;

    }

    if(-1==(fifo->fd=openat(
        parent->fd,
        name,
        O_RDWR|O_NONBLOCK
    ))){
        perrno(
            "dir_node->new_fifo->openat { parent:%s , self : %s }\n",
            parent ? parent->name : "none" ,
            name
        );
                
        dir_node_free(fifo);

        return NULL;
    }

    return fifo;
}
static h_dir_node  dir_node_new_channel(    h_dir_node parent, cstr name,int oflags){
    h_dir_node      ch_dir=NULL,temp=NULL;
    bool            success=TRUE;
    char            buff[8]="rx.fifo";

    //sanity check
    if( !parent || parent->fd==-1 || parent->type!=e_dir  ){
        errno=EINVAL;
        perrno("%s\n","dir_node->new_channel->arg");

        return NULL;
    }

    //init dir
    if(!(ch_dir=dir_node_new_dir(parent,name,oflags)))
        return NULL;
    
    //set attribute channel
    ch_dir->type=e_channel;

    int mask=(
        oflags&O_CREAT
        ?   oflags
        :   oflags & ~(O_RDONLY|O_RDWR)
    );
    for(int i=0;i<2 &&success;++i){
        
        if(!(temp=dir_node_new(ch_dir,buff,e_fifo,mask) )){
            dir_node_free(ch_dir);
            return NULL;
        }
        perrno_if_not_true(
            (success&=list_push_tail(ch_dir->childs,temp)),
            "dir_node->new_channel->push {parent:%s,self:%s}",
            parent->name,name
        );
        *buff='t';
        mask=(
            oflags&O_CREAT
            ?   oflags
            :   oflags & ~(O_WRONLY|O_RDWR)
        );
    }
    if(!success){ 
        
        perrno("%s\n","dir_node->new_channel->new fifo");
        dir_node_free(ch_dir);
        
        return NULL;
    }


    return ch_dir;

}
static h_dir_node  dir_node_new_file(       h_dir_node parent, cstr name,int oflags){
    h_dir_node  nd=NULL;
    int flags=( oflags & O_CREAT )? oflags|O_EXCL:oflags;

    //doit etre relatif
    if(!parent||parent->fd==-1||!name){
        perrno(
            "dir_node->new_file->arg {parent:%s,self:%s}\n",
            parent?parent->name:"none",name
        );
        return NULL;
    }

    //init default
    if(!(nd=dir_node_new_default(parent,name,e_file)))
        return NULL;

    if(-1==(nd->fd=openat(parent->fd,name,flags))){
        perrno(
            "dir_node->new_file->openat {parent:%s,self:%s}\n",
            parent->name,name
        );
        dir_node_free(nd);

        return NULL;
    }

    return nd;
}
static h_dir_node  dir_node_new_virtual(    h_dir_node parent, cstr name,int oflags){
    h_dir_node nd=NULL;

    if(!(nd=dir_node_new_default(parent,name,e_virtual)))
        return NULL;

    return nd;
}
static h_dir_node  dir_node_new_lock(       h_dir_node parent, cstr name,int oflags){
    h_dir_node  nd      =(h_dir_node)NULL;
    str         bufname =(str)NULL;

    //on regarde si on est au startup du serveur par O_NOFOLLOW
    oflags=( oflags&O_NOFOLLOW ? oflags&~O_NOFOLLOW | O_CREAT | O_EXCL : oflags );

    //le parent d'un lock peut ne pas etre un directory dans le cas dun fifo
    if(!parent||parent->type==e_virtual){
        //cree le nom du lock
        bufname=fstr_cashvariade_formater_make("/tmp/"$(nm)".lock",$(nm),name);
    }else{
        if(parent->type==e_fifo){
            //on change lext
            bufname=fstr_remplacer_make(name,".fifo",".lock");
        }else{
            //sinon on append le nom
            bufname=fstr_remplacer_make($(nm)".lock",$(nm),name);
        }
    }

    //si pas de parent, on a un path absolu
    if(!parent||parent->fd==-1){

        //init default
        if(!(nd=dir_node_new_default(NULL,bufname,e_lockf)))
            return NULL;

        //open 
        if(-1==(nd->fd=open(bufname,oflags))){
            perrno(
                "dir_node->new_lock->open {parent:%s,self:%s}\n",
                parent? parent->name :"none" , 
                bufname
            );
            dir_node_free(nd);
            return NULL;
        }

    }else{// sinon on est en relatif

        h_dir_node real_parent=parent->type==e_fifo?parent->parent:parent;

        //init default
        if(!(nd=dir_node_new_default(NULL,bufname,e_lockf)))
            return NULL;

        nd->fd=openat(real_parent->fd,bufname,oflags,0777);
        //om ouvre
        if(-1==(nd->fd)){
            perrno(
                "dir_node->new_lock->open {parent:%s,self:%s} \n",
                parent->name,name
            );
            dir_node_free(nd);
            free(bufname);
            return NULL;
        }
    }
    //cleanup
    free(bufname);

    //set properties
    nd->parent=parent;
    if(parent) 
        parent->lock=nd;
    nd->remove_at_free= ( oflags & O_CREAT )? TRUE : FALSE ;
    nd->lock=NULL;
    
    return nd;
}
static h_dir_node  dir_node_new_dir( h_dir_node parent, cstr name,int oflags){
    h_dir_node nd=NULL;
    bool server_start=oflags&O_NOFOLLOW?TRUE:FALSE;
    oflags&=~O_NOFOLLOW;
    
    //must attach to a root
    if(!parent||(parent->fd==-1&&parent->type!=e_virtual))
        return NULL;

    //init default
    if(!(nd=dir_node_new_default(parent,name,e_dir)))
        return NULL;

    //si on veut le creer
    if(oflags&O_CREAT){
        //mkdir
        if(-1==(
            (parent->type==e_virtual)
            ?   mkdir(name,0777)
            :   mkdirat(parent->fd,name,0777)
        )){
            perrno(
                "dir_node->new_dir->mkdir {parent:%s,self:%s}\n",
                "none",name
            );
            dir_node_free(nd);

            return NULL;
        }
        //open the inode
        if(-1==(nd->fd=(
            (parent->type==e_virtual)               /*si le parent est virtuel*/
            ?   open(name,O_DIRECTORY)              /*on call open*/
            :   openat(parent->fd,name,O_DIRECTORY) /*sinon on va relatif*/
        ))){
            perrno(
                "dir_node->new_dir->open {parent:%s,self:%s}\n",
                parent?parent->name:"none",name
            );
            nd->remove_at_free=TRUE;
            dir_node_free(nd);
            return NULL;
        }

    }
    else{
        //si on veut blocking(attendre quil existe existe)
        int locked=dir_node_lock(
            parent,
            LOCK_SH,
            (oflags&O_NONBLOCK)? 0 : OPTLOCK_TRY 
        );
        //si le lock a marche
        if( !locked&&(O_NONBLOCK&oflags) || locked ){
            nd->fd=(
                (parent->type==e_virtual)               /*si le parent est virtuel*/
                ?   open(name,O_DIRECTORY)              /*on call open*/
                :   openat(parent->fd,name,O_DIRECTORY) /*sinon on va relatif*/
            );
            //on ouvre
            if(  nd->fd == -1){
                perrno(
                    "dir_node->new_dir->open {parent:%s,self:%s}\n",
                    parent->name,name
                );
                dir_node_free(nd);
                errno=ENOENT;
                return NULL;
            }
            //on unlock
            dir_node_lock(parent,LOCK_UN,0);
        }
        else{
            errno=ENOENT;
            perrno(
                "dir_node->new_dir->wait for parent {parent:%s,self:%s}\n",
                parent->name,name
            );
            dir_node_free(nd);

            return NULL;
        }
    }

    return nd;
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                                FILE_SYSTEM PUBLIQUES
//========================================================================================
h_private_file_system private_file_system_open(void){
    cstr                  db_path =getenv("IMDB_DATA_PATH");
    h_private_file_system pr=NULL;
    bool                  success=TRUE;
    int                   i=-1;

    if(!(pr=calloc(1,sizeof(t_private_file_system)))){
        free(db_path);
        return NULL;
    }
    if(!(   (pr->public_fs.channels)=calloc(
        2*(NB_CHANNEL+1),
        sizeof(h_dir_node)
    ))){
        free(db_path);
        free(pr);
        return NULL;
    }

    while(success&&i<8){
        ++i;
        str tmp_filename=NULL;
        switch(i){
            case 0:{
                    success&=((
                        pr->root=dir_node_new(
                            NULL,
                            "E216_SERVER_ROOT",
                            e_virtual,
                            O_CREAT|O_EXCL
                        ))!=NULL
                    );
                    dir_node_lock(pr->root,O_EXCL,0);
                }break;
            case 1:{
                    success&=((
                        pr->data_store=dir_node_new(
                            pr->root,
                            db_path,
                            e_dir,
                            O_RDWR|O_NONBLOCK|SERVER_START
                        ))!=NULL
                    );
                }break;
            case 2:{    
                    tmp_filename=(str)FICHIER_TITRE;
                }
            case 3:{
                    tmp_filename=tmp_filename?tmp_filename:(str)FICHIER_NOMS;
                }
            case 4:{    
                    tmp_filename=tmp_filename?tmp_filename:(str)FICHIER_COTE;
                    success&=((
                        pr->db_files[i-2]=dir_node_new(
                            pr->data_store,
                            tmp_filename,
                            e_file,   
                            O_RDWR|SERVER_START
                        ))!=NULL
                    );
                }break;
            case 5:{
                    success&=((
                        pr->server_node=dir_node_new(
                            pr->root,
                            "/tmp/e216_server",
                            e_dir,
                            O_CREAT|O_EXCL|O_NONBLOCK
                        ))!=NULL
                    );
                    if(pr->server_node)
                        pr->server_node->remove_at_free=TRUE; 
                }break;
            case 6:{
                    success&=((
                        pr->public_fs.channels_dir=dir_node_new(
                            pr->server_node,
                            "channels"        ,
                            e_dir,
                            O_CREAT|O_EXCL
                        ))!=NULL
                    );
                    if(success) 
                        pr->public_fs.channels_dir->remove_at_free=TRUE;
                }break;
            case 7:{
                    str template=strdup("ch_xx");
                    for(int i=0;i<=NB_CHANNEL;++i){
                        h_dir_node  tmp=NULL;
                        
                        sprintf(template,"ch_%02d",i);

                        success&=((
                            tmp=dir_node_new(
                                pr->public_fs.channels_dir,
                                template,
                                e_channel,
                                O_EXCL|O_CREAT|O_NONBLOCK|O_RDWR
                            ))!=NULL
                        );
                        perrno_if_not_true(success,"%d\n",i);
                        if(success){
                            pr->public_fs.channels[RXID(i)]=list_get_elem_index(
                                tmp->childs,0
                            );
                            pr->public_fs.channels[TXID(i)]=list_get_elem_index(
                                tmp->childs,1
                            );
                        }
                    }
                    free(template);
                }
            default: break;
        }
    }
    if(!success){
        if(i)
            dir_node_free(pr->root);
        free(pr->public_fs.channels);
        free(pr);
        return NULL;
    }

    return pr;
};
//-------------------------------------------------
void private_file_system_close(void* priv){
    h_private_file_system pr=priv;
    if(!priv)
        return;
    dir_node_free(pr->root);
    free(pr->public_fs.channels);
    free(pr);
    pr=NULL;
}
//-------------------------------------------------
h_public_file_system public_file_system_open(void){
    h_public_file_system pub=NULL;
    bool success=TRUE;
    time_t tim=time(NULL);
    struct tm* stm=gmtime(&tim);

    str bufrootname=strdup("E216_CX_%02d_%02d_%02d]");
    sprintf(bufrootname,"E216_CX_%02d_%02d_%02d]",stm->tm_hour,stm->tm_min,stm->tm_sec);

    if(!(pub=calloc(1,sizeof(t_public_file_system))))
        return NULL;

    if(!(pub->channels=calloc(2*NB_CHANNEL+2,sizeof(h_dir_node)))){
        errno=ENOMEM;
        free(pub);
        return NULL;
    }

    if(!(   pub->root=dir_node_new(NULL,bufrootname,e_virtual,O_CREAT|O_EXCL|O_NONBLOCK))){
        free(pub->channels);
        free(pub);
        free(bufrootname);
        return NULL;
    }
    free(bufrootname);
    h_dir_node tmp=NULL;
    if(!(tmp=dir_node_new(pub->root,"/tmp/e216_server",e_dir,O_NONBLOCK))){
        dir_node_free(pub->root);
        free(pub->channels);
        free(pub);

        return NULL;
    }
    if(!(   pub->channels_dir=dir_node_new(tmp,"channels",e_dir,O_NONBLOCK))){
        dir_node_free(pub->root);
        free(pub->channels);
        free(pub);
        return NULL;
    }

    str template=strdup("ch_xx");
    for(int i=0;i<=NB_CHANNEL;++i){
        h_dir_node  tmp=NULL;
        
        sprintf(template,"ch_%02d",i);

        if(!(tmp=dir_node_new(
            pub->channels_dir,template,e_channel,0
        ))){
            free(template);
            dir_node_free(pub->root);
            free(pub->channels);
            free(pub);
            return NULL;
        }
        pub->channels[RXID(i)]=list_get_elem_index(tmp->childs,0);
        pub->channels[TXID(i)]=list_get_elem_index(tmp->childs,1);
    }
    
    return pub;
}
//-------------------------------------------------
void public_file_system_close(void* pub){
    h_public_file_system p=pub;
    if(!pub)
        return;
    
    dir_node_free(p->root);

    free(p->channels);

    free(p);

}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
