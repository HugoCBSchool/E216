/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*
    Projet      : ELE216 - Laboratoire 1
    Date        : 09 fevrier 2021
    Par         : Gabriel Gandubert et Hugo Cusson-Bouthillier
    Definition  : Manipulation des chaines de caracteres de ligne provenant des bases
                  de donnees du programme.

    Contient:
        TYPES:
            - structure opaque t_table_string et son handle h_table_string
            - t_fp_foreach: signature de foncteur pour les fonctions de type foreach

        FONCTIONS PUBLIQUES:
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
            
        FONCTIONS PRIVEES
            - liberer_paire             : Permet de liberer une paire d'une table.
            - hmap_get_paire            : Permet de trouver la paire dans le hmap selon la
                                            cle.
            - hmap_get_paire_precedente : Permet de trouver la paire precedente dans le 
                                            hmap selon la cle de l'element voulu.
            - hash                      : Permet de trouver le numero de table selon la
                                            cle.
*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Header file
#include "util/hashmap.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                          CONSTANTES D'IMPLEMENTATION                                 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Constante de dimension de la Hmap.
#ifndef HMAP_TAILLE
#define HMAP_TAILLE 1001
#endif

// Constante de longeur des clees des elements du Hmap.
#ifndef HMAP_CLE_STR_LEN
#define HMAP_CLE_STR_LEN 9
#endif

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                          TYPES ET STRUCTURES OPAQUES                                 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief   type t_paire et son handle h_paire represent la paire de la cle et des donnees 
 *          utilisee par le Hmap.
 */
typedef struct paire t_paire, *h_paire;

/**
 * @brief   La structure paire contient la cle, les donnees, la prochaine paire et un
 *          pointeur de fonction pour la liberation des donnees contenu.
 */
struct paire{
  char                      *cle;
  void                      *donnee;
  h_paire                   next;
  fp_dtor liberer_data;
};

/**
 * @brief   Structure de la tete de la Hmap qui a une taille limite, une taille courante
 *          et les donnees a contenir.
 */
struct hmap {
    int64_t   taille_table;
    h_paire   table_paire[HMAP_TAILLE];
};

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                          DECLARATIONS FONCTIONS PRIVEES                              */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief   Fonction de liberation d'une paire du Hmap.
 * @param   paire : la paire de cle et de donnees d'une cellule du Hmap.
 * @retval  None.
 */
void    liberer_paire(  h_paire paire);

/**
 * @brief   Fonction pour trouver la prochaine paire dans le Hmap selon la cle donnee.
 * @param   map : la Hmap a explorer.
 * @param   cle : Chaine de caractere de la cle.
 * @retval  la paire de donnees trouvee. En cas d'erreur, elle retourne un pointeur nul.
 */
h_paire hmap_get_paire(h_hmap map, const char *cle);

/**
 * @brief   Fonction pour trouver la paire precedente dans le Hmap selon la cle donnee.
 * @param   map : la Hmap a explorer.
 * @param   cle : Chaine de caractere de la cle.
 * @retval  la paire de donnees trouvee. En cas d'erreur, elle retourne un pointeur nul.
 */
h_paire hmap_get_paire_precedente(h_hmap map, const char *cle);

/**
 * @brief   Fonction pour trouver la position de la cle dans le Hmap.
 * @param   map : la Hmap a explorer.
 * @param   cle : Chaine de caractere de la cle.
 * @retval  la position de la cle dans le Hmap sur 8 bytes. S'il y a une erreur, elle
            retourne -1.
 */
int64_t hash(h_hmap map,const char *cle);

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                          IMPLLEMENTATION FONCTIONS PUBLIQUES                         */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Creation du Hmap.
h_hmap hmap_creer( ){

    h_hmap map=NULL;

    // Allocation de la struct du map
    if( !(  map = (h_hmap)malloc(sizeof(t_hmap)) )){
        return NULL;
    }

    // Initialisation des valeurs.
    map->taille_table=HMAP_TAILLE;
    memset(map->table_paire,0,sizeof(h_paire)*HMAP_TAILLE);

    // Retourne le Hmap produit.
    return map;
}

//----------------------------------------------------------------------------------------

// Ajouter un element au Hmap.
int hmap_add(   h_hmap const        map     ,   
                const  char*  const cle     , 
                void*               valeur  ,
                const  fp_dtor const f_liberer_valeur
){
    // Initialisation de variables locales pour traitement et position de la cle.
    void*    val = NULL;
    h_paire pair = NULL;
    int64_t hashval=hash(map,cle);

    // Gestion des erreurs de la fonction hash.
    if(hashval<0)
        return EXIT_FAILURE;

    // Si on ne le trouve pas dans la table
    if( !( pair = hmap_get_paire( map , cle ) ) ){

        // On cree un nouveau noeud
        if( !( pair = (h_paire) malloc( sizeof(t_paire) ) ) )  
            return EXIT_FAILURE;

        // On popule
        if( !( pair->cle = strdup(cle) ) ){
            free(pair);
            return EXIT_FAILURE;
        }
        if(valeur){
            pair->donnee=valeur;
            pair->liberer_data=f_liberer_valeur;
        }else{
            free(pair);
            return EXIT_FAILURE;
        }
        pair->next = NULL;                  //popule le next
        //si on a la place au root
        if(!map->table_paire[hashval])
            map->table_paire[hashval] = pair;   //popule le bucket du hash
        else{
            //sinon on trouve la place dans la chaine
            h_paire p=map->table_paire[hashval];
            while(p->next){
                p=p->next;
            }
            p->next=pair;
        }
    } 

    // Si on le trouve dans la table.
    else {
        // Si vide, on popule.
        if(!pair->donnee){
            if( valeur ){
                pair->donnee=valeur;
                pair->liberer_data=f_liberer_valeur;
            } else {
                return EXIT_FAILURE;
            }
        }else{
            return EXIT_FAILURE;
        }
    }
    return (EXIT_SUCCESS);
}

//----------------------------------------------------------------------------------------

// Remplace ou ajoute un element au Hmap.
int hmap_remplacer( h_hmap const        map     ,
                    const  char*  const cle     , 
                    void*               valeur  ,
                    const  fp_dtor const f_liberer_valeur 
){
    // Initialisation de variables locales pour traitement et position de la cle.
    void*   val = NULL;
    h_paire pair = NULL;
    int64_t hashval=hash( map , cle );

    // Gestion des erreurs de la fonction hash.
    if(hashval<0)
        return EXIT_FAILURE;

    // Si on ne le trouve pas dans la table
    if( !( pair = hmap_get_paire( map , cle ) ) ){

        // On cree un nouveau noeud
        if( !( pair = (h_paire) malloc( sizeof(t_paire) ) ) )  
            return  EXIT_FAILURE;

        // On popule
        if( !( pair->cle = strdup(cle) ) ){
            free(pair);
            return EXIT_FAILURE;
        }

        // Si la valeur est valide, on l'ajoute les donnees.
        if(valeur){
            pair->donnee=valeur;
            pair->liberer_data=f_liberer_valeur;
        }else{
            free(pair);
            return EXIT_FAILURE;
        }
        pair->next = NULL;                  //popule le next

        //si on a la place au root
        if(!map->table_paire[hashval])
            map->table_paire[hashval] = pair;   //popule le bucket du hash
        else{
            //sinon on trouve la place dans la chaine
            h_paire p=map->table_paire[hashval];
            while(p->next){
                p=p->next;
            }
            p->next=pair;//on popule le next
        }
    } else {
		// Si on le trouve dans la table
		// Si les donnees n'existe pas, on popule.
        if(!pair->donnee){

            if( valeur ){
                pair->donnee=valeur;
                pair->liberer_data=f_liberer_valeur;
            } else 
                return EXIT_FAILURE;

        } else {// Si elles existes, on les retires et on les remplaces.

            if(pair->liberer_data){
                pair->liberer_data(pair->donnee);
            }
            pair->donnee=NULL;
            
            if( valeur ){
                pair->donnee=valeur;
                pair->liberer_data=f_liberer_valeur;
            } else 
                return EXIT_FAILURE;
        }
    }
    
    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------------------

// Sortir les donnees positionnees par la cle.
void *hmap_get( h_hmap const map, const char *const   cle ){

    h_paire pair=hmap_get_paire(map,cle); // Trouve la paire dans le Hmap.

	// Si les donnees existes, on les returnes.
    return ( 
        pair 
        ?   (pair->donnee)
        :   NULL
    );
}

//----------------------------------------------------------------------------------------

// Retire la paire du Hmap et retourne les donnees de l'element retiree.
void* hmap_pop_element( h_hmap const map, const char *const cle){
    
    // Trouver la paire precedente pour le pas perdre la reference.
    h_paire pair=hmap_get_paire_precedente(map,cle);

    // Verification des parametres d'entree.
    if(!pair){
        return NULL;
    }

    // Pointeur temporaire vers la valeur suivante.
    h_paire temp=pair->next;

    // Si une valeur existe apres l'element a enlever, le brancher a l'element precedent.
    if(pair->next->next){
        pair->next=temp->next;
    }

    // Recupere la donnee de l'element a retirer et la retourne.
    void* retour=temp->donnee;
    free(temp->cle);
    free(temp);

    return retour;
}

//----------------------------------------------------------------------------------------

// Liberation d'un element du Hmap.
void hmap_supprimer_element( h_hmap const map, const char *const cle){
    
    // Trouver la paire precedente pour le pas perdre la reference.
    h_paire pair=hmap_get_paire_precedente(map,cle);
    h_paire cible=NULL;
    int64_t hval=hash(map,cle);

    // Gestion des erreurs de la fonction hash.
    if(hval<0){
        return;
    }

    // Si aucune paire n'est trouvee.
    if(!pair){

        // Verifier si la paire serait une premiere de ligne.
        if(!(pair=hmap_get_paire(map,cle))){
            return;
        }
        // Si une premiere de ligne, liberer la paire.
        if(pair==map->table_paire[hval]){
            if(pair->next){
                map->table_paire[hval]=pair->next;
                liberer_paire(pair);
            }else{
                liberer_paire(pair);
                map->table_paire[hval]=NULL;
            }
        }

        // NE DEVRAIT JAMAIS ARRIVER! Mais, indice de cause d'erreur ajoutee.
        else{
            printf("MEMORY LEAK ASSERTED IN HMAP: UNRELATED PAIR IN GRAPH.\n");
        }
    }else{ // Si on trouve une paire, liberer la paire cible.
        cible=pair->next;

        if(cible->next){
            pair->next=cible->next;
        }

        liberer_paire(cible);
    }
}

//----------------------------------------------------------------------------------------

// Fonction permettant d'effectuer une fonction pointee a tous les elements du Hmap.
int hmap_iterer_paires( h_hmap const     map, 
                        fp_ctx_cle_val   func, 
                        void *const      contexte)
{

    // Verification des parametres.
    if(!map || !func || !map->table_paire)
        return EXIT_FAILURE;

    // Pour tout le Hmap. effectuer la fonction.
    for(int i=0;i<HMAP_TAILLE;++i){

        for(    h_paire p = map->table_paire[i];
                p!=NULL;
                p=p->next
        ){
            if( p->donnee && p->cle )
                func(contexte,p->cle,p->donnee);
        }
    }

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------------------

/* 
 * Fonction permettant d'effectuer une fonction pointee a tous les elements du Hmap selon
 * les criteres d'une autre fonction pointee.
 */
int hmap_iterer_executer_si(    h_hmap const                map,
                                fp_ctx_cle_val              f_exec,
                                fp_ctx_cle_val_return_exit  f_si,
                                void* const                 contexte_exec,
                                void* const                 contexte_si
){
	// Verification des parametres.
    if(!map || !f_exec || !f_si || !map->table_paire )
        return EXIT_FAILURE;

    // Pour tout le Hmap, verifier la condition et effectuer la fonction.
    for(int i=0;i<HMAP_TAILLE;++i){

        for(    h_paire p = map->table_paire[i];
                p!=NULL;
                p=p->next
        ){
            if( p->donnee && p->cle ){

                if(f_si(contexte_si,p->cle,p->donnee)==EXIT_SUCCESS){
                    f_exec(contexte_exec,p->cle,p->donnee);
                }
			}
        }
    }

    return EXIT_SUCCESS;
}
int hmap_iterer_paires_supprimer_si(   h_hmap const                map,
                                       fp_ctx_cle_val_return_exit  f_si,
                                       void* const                 contexte
){
    if(!map ||  !f_si || !map->table_paire )
        return EXIT_FAILURE;
    
    //pour tous les head de toutes les chaines
    for(int i=0;i<HMAP_TAILLE;++i){

        //on veut connaitre la precedente si on supprime
        h_paire precedent=NULL;
        h_paire suivant=NULL;
        int goto_next=TRUE;
        //pour toutes les paires dans la chaine
        for(    h_paire p = map->table_paire[i];
                p!=NULL;
                p=suivant
        ){
            //si la paire est non nule et la cle aussi
            if( p->donnee && p->cle ){

                //on appelle la fonction de validation, supprime sur EXIT_SUCCESS
                if(f_si(contexte,p->cle,p->donnee)==EXIT_SUCCESS){
                    h_paire cible=p;        //temporaire du noeud a delete
                    suivant=p->next;        //sauvegarde le prochain avant delete
                    
                    //si la precedente est nulle on est le head
                    if(!precedent){
                        liberer_paire(cible);           //libere le head
                        map->table_paire[i]=suivant;    //update le head
                    }else{
                        precedent->next=suivant;        //update le precedent
                        liberer_paire(cible);           //libere la paire
                    }

                }else{
                    suivant=p->next; //on continue down the chain
                    precedent=p;
                }
            }else{
                //impossible
                printf("FATAL ERROR:HASHMAP EMPTY PAIR CONSERVED.\n");
                suivant=p->next;
                precedent=p;
            }
        }
    }
}
//----------------------------------------------------------------------------------------
// Retourne la taille du Hmap.
int64_t hmap_get_taille(h_hmap const map){
    return(
        map
        ?   map->taille_table
        :   -1
    );
}

//----------------------------------------------------------------------------------------

// Libere le Hmap.
void hmap_liberer(h_hmap map){
    
    // Verification des parametres.
    if(!map) return;

    // Si la table existe, la liberer.
    if(map->table_paire){

        // Pour tout le Hmap, liberer la table des paires.
        for(int i=0;i<HMAP_TAILLE;++i){

            h_paire precedent=map->table_paire[i];

            if(precedent){

                while(precedent->next){

                    h_paire actuel=precedent->next;
                    liberer_paire(precedent);
                    precedent=actuel;
                }
            }

            liberer_paire(precedent);
            map->table_paire[i]=NULL;
        }
    }

    // Liberation final du Hmap.
    free(map);
    map=NULL;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                          IMPLEMENTATION FONCTIONS PRIVEES                            */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Permet de liberer une paire d'une table.
h_paire hmap_get_paire(h_hmap map, const char *cle){

    // Trouver la position de la cle dans le Hmap.
    int64_t hashval=hash(map,cle);

    // Gestion des erreurs de la fonction hash.
    if(hashval<0)
        return NULL;

    // Scan de l'adresse donnee par la cle pour trouver la bonne cle dans le Hmap.
    for(    h_paire np = map->table_paire[hashval]; 
            np != NULL;
            np = np->next
    ){
        if(strcmp(np->cle, cle)==0) return np;
    }

    // Si on ne la trouve pas, erreur.
    return NULL;
}

//----------------------------------------------------------------------------------------

// Permet de trouver la paire dans le hmap selon la cle.
h_paire hmap_get_paire_precedente(h_hmap map, const char *cle){

    // Trouver la position de la cle dans le Hmap.
    int64_t hashval=hash(map,cle);

    // Gestion des erreurs de la fonction hash.
    if(hashval<0)
        return NULL;

    // Scan de l'adresse donnee par la cle pour trouver la bonne cle dans le Hmap.
    for(    h_paire np = map->table_paire[hashval]; 
            np->next!= NULL;
            np = np->next
    ){
        if(strcmp(np->next->cle, cle)==0) return np;
    }

    // Si on ne la trouve pas, erreur.
    return NULL;
}

//----------------------------------------------------------------------------------------

/* 
 * hmap_get_paire_precedente : Permet de trouver la paire precedente dans le hmap selon 
 * la cle de l'element voulu.
 */
void liberer_paire(  h_paire paire){
    
    // La paire ne doit pas etre nul.
    if(!paire)      
        return;

    // Liberer la cle si elle existe.
    if(paire->cle)  
        free(paire->cle);

    // Si la fonction de liberation existe et les donnees existes.
    if(paire->liberer_data && paire->donnee)
        paire->liberer_data(paire->donnee);
    
    // Liberation de la paire.
    free(paire); 
}

//----------------------------------------------------------------------------------------

// Permet de trouver le numero de table selon la cle.
int64_t hash(h_hmap map,const char *cle){
    if( !map || !cle || 
        (strlen(cle)!=HMAP_CLE_STR_LEN&&strlen(cle)!=1+HMAP_CLE_STR_LEN)
    ){

        #ifdef __DEBUG_HASHMAP
        if(!map)
            printf("Hash debug: null map arg\n");
        if(!cle)
            printf("Hash debug: null cle arg\n");
        else
            printf("hash debug: strlen(cle)=%lu",strlen(cle));  
        #endif

        return -1;
    }
    long long int hash=0;
    hash = ( hash + ( ( cle[0] >> 3 ) << 1 ) + ( cle[0] % 2 )  ) << 4;

    #ifdef __DEBUG_HASHMAP
    printf("hash debug: result step 1= %lld\n",hash);
    #endif

    hash = ( hash + ( ( cle[1] >> 3 ) << 2 ) + ( cle[1] % 4 )  ) << 0;

    #ifdef __DEBUG_HASHMAP
    printf("hash debug: result step 2= %lld\n",hash);
    #endif

    for( int i=0 ; cle[i] ; ++i ){
        hash = ( hash << 4 ) + cle[i] % 16;
    }

    #ifdef __DEBUG_HASHMAP
    printf("hash debug: result step 3= %lld\n",hash);
    #endif

    return (hash % map->taille_table );
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
