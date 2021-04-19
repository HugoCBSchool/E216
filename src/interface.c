/*
	Projet		: ELE216 - Laboratoire 1
	Date		: 27 janvier 2021
	Par 		: Gabriel Gandubert et Hugo Cusson-Bouthillier
	Logiciel	: Sublime text & Eclipse
	Definition	: Lecture de chaque ligne de la base de donnee.

	Contient:
		Fait appel aux librairies parser.h et resultats.h pour traiter les lignes.

		FONCTIONS PUBLIQUES:
			% Ouverture et recherche du fichier texte.
			int recherche_fichier(str nom_fichier, pt_critere critere)

		FONCTIONS PRIVEEs :
			% Fonction privee permettant de verifier si le fichier recu est correct 
			% avant ouverture.
			int verifier_fichier(str nom_fichier)

*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Header file
#include "interface.h"
#include "base.h"
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							CONSTANTES D'IMPLEMENTATION     							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

//fichiers de sortie

#ifndef __DEBUG
static const char FICHIER_SORTIE_NOM[]			="resultat_nom.txt";
static const char FICHIER_SORTIE_TITRE[]		="resultat_titre.txt";
static const char FICHIER_SORTIE_COTE[]			="resultat_cote.txt";
static const char FICHIER_SORTIE_RESULTATS[]	="resultats.txt";

#endif

// Caratere de separation de l'adresse du fichier.
#define PATH_SEPARATOR '/'
#define TOKEN_TITRE_ID ','
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							TYPES ET STRUCTURES OPAQUES     							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
typedef struct op_supprimer_args{
	h_list list_titre;
	h_list list_nom;
	h_list list_cote;
	h_list list_id_titre;
}t_op_supprimer_args;

typedef struct search_thread_args{
	str filename;
	pt_critere crit;
	fileFD	fd;
}t_search_thread_args;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							DECLARATIONS FONCTIONS PRIVEES    							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief  	Permet de verifier le type de base de donnees et d'indiquer
 *			si elle est inconnue par le programme.
 * @note	HLR15 : Verification du nom du fichier avant ouverture. Les messages 
 *				d'erreur sont generes par la fonction verifier_fichier.
 * @param  	nom_fichier	:	la string d'adresse de la base de donnee.
 * @retval 	Le type de fichier donnee selon l'enum enum_t_fichier. retourne ERREUR si
 *			le fichier est inconnue ou NULL.
 */
t_tag_fichier verifier_fichier(str nom_fichier);

/**
 * @brief  	Fonction de type f_contexte_cle_valeur reconnue par l'iteratrice de hashmap.h
 * 			nommee hmap_iterer_contexte_cle_valeur. 
 * 			Cette routine permet d'ajouter les titres de la map de 
 * 			titres a la map de resultats.
 * 
 * @note	NE PAS UTILISER CETTE FONCTION AUTREMENT QUE COMME ARGUMENT A 
 * 				hmap_iterer_contexte_cle_valeur. EMPLOYEE SUR UN h_hmap CONTENANT DES 
 * 				h_donnee_titre. AUTREMENT, UN SEGFAULT EST INEVITABLE.
 * 
 * @param  contexte: 	le h_hmap de la map de resultats.
 * @param  cle: 		recipiendaire de la cle passee par l'iteratrice appelante
 * @param  valeur: 		recipiendaire de la valeur passee par l'appelante. 
 * 							celle-ci est connue de la fonction comme etant un 
 * 							h_donnee_titre implicite.
 * @retval None
 */
extern void ajouter_resultat_titre_hmap(	void* 		contexte	, 
											cstr const 	cle			, 
											void* 		valeur		);

/**
 * @brief  	Fonction de type f_contexte_cle_valeur reconnue par l'iteratrice de hashmap.h
 * 			nommee hmap_iterer_contexte_cle_valeur. 
 * 
 * 			Cette routine permet a tous les noms de m_s_resultats_recherche.resultat_nom
 * 			de rechercher les h_donnee_resultat correspondant a un titre de son tuple de 
 * 			titres dans m_s_resultats_recherche.resultat_intersection et d'y changer le 
 * 			champ de nom.
 * 
 * @note	NE PAS UTILISER CETTE FONCTION AUTREMENT QUE COMME ARGUMENT A 
 * 				hmap_iterer_contexte_cle_valeur, EMPLOYEE SUR UN h_hmap CONTENANT DES 
 * 				h_donnee_nom. AUTREMENT, UN SEGFAULT EST INEVITABLE.
 * 
 * @param  contexte: 	le h_hmap de la map de resultats.
 * 
 * @param  cle: 		recipiendaire de la cle passee par l'iteratrice appelante 
 * 						correspondant au string du id_titre a chercher.
 * 
 * @param  valeur: 		recipiendaire de la valeur passee par l'appelante. celle-ci est 
 * 						connue de la fonction comme etant un h_donnee_nom implicite.
 * @retval None
 */
extern void ajouter_resultat_nom_hmap(	void* 				contexte	, 
										cstr const 	cle			, 
										void* 				valeur		);

/**
 * @brief  	Fonction de type f_contexte_cle_valeur reconnue par l'iteratrice de hashmap.h
 * 			nommee hmap_iterer_contexte_cle_valeur. 
 * 
 * 			routine permet a tous le element de m_s_resultats_recherche.resultat_cote
 * 			de rechercher les h_donnee_resultat correspondant a son id_titre dans 
 * 			m_s_resultats_recherche.resultat_intersection et d'y changer le champ de cote.
 * 
 * @note	NE PAS UTILISER CETTE FONCTION AUTREMENT QUE COMME ARGUMENT A 
 * 				hmap_iterer_contexte_cle_valeur, EMPLOYEE SUR UN h_hmap CONTENANT DES 
 * 				h_donnee_cote. AUTREMENT, UN SEGFAULT EST INEVITABLE.
 * 
 * @param  contexte: 	le h_hmap de la map de resultats.
 * 
 * @param  cle: 		recipiendaire de la cle passee par l'iteratrice appelante 
 * 						correspondant au string du id_titre a chercher.
 * 
 * @param  valeur: 		recipiendaire de la valeur passee par l'appelante. celle-ci est 
 * 						connue de la fonction comme etant un h_donnee_cote implicite.
 * @retval none
 */
extern void ajouter_resultat_cote_hmap(	void* 				contexte	, 
										cstr const 	cle			, 
										void* 				valeur		);

/**
 * @brief  	Fonction de type f_si_contexte_cle_valeur reconnue par l'iteratrice de 
 * 			hashmap.h nommee hmap_iterer_contexte_cle_valeur. 
 * 
 * 			routine permet a tous le element de m_s_resultats_recherche.resultat_cote
 * 			de rechercher les h_donnee_resultat correspondant a son id_titre dans 
 * 			m_s_resultats_recherche.resultat_intersection et d'y changer le champ de cote.
 * 
 * @note	NE PAS UTILISER CETTE FONCTION AUTREMENT QUE COMME ARGUMENT A 
 * 				hmap_iterer_contexte_cle_valeur, EMPLOYEE SUR UN h_hmap CONTENANT DES 
 * 				h_donnee_cote. AUTREMENT, UN SEGFAULT EST INEVITABLE.
 * 
 * @param  contexte: 	le h_hmap de la map de resultats.
 * 
 * @param  cle: 		recipiendaire de la cle passee par l'iteratrice appelante 
 * 						correspondant au string du id_titre a chercher.
 * 
 * @param  valeur: 		recipiendaire de la valeur passee par l'appelante. celle-ci est 
 * 						connue de la fonction comme etant un h_donnee_cote implicite.
 * @retval None
 */
extern int si_resultat_invalide_success(	void* 				contexte, 
											cstr const 	cle, 
											void* 				valeur);

/**
 * @brief  
 * @note   
 * @param  contexte: 
 * @param  cle: 
 * @param  valeur: 
 * @retval None
 */
extern void ajouter_resultat_lignes(
	void* 		contexte, 
	cstr const 	cle, 
	void* 		valeur
);
extern void ajouter_resultat_id_titre_list(void* contexte,	str   cle,void* valeur);
/**
 * @brief  Permet de faire l'union des 3 recherches de fichier.
 * @note   HLR30: Intersection des résultats
 *			Si les resultats gardes par linterface sont bons	
 * @retval 	EXIT_SUCCESS 	si on avait des resultats pour tous les criteres et 
 * 							que l'union a fonctionnee
 * 			EXIT_FAILURE	sinon.
 */
h_hmap analyser_resultats(h_hmap titre, h_hmap nom, h_hmap cote, pt_critere crit);

h_hmap recherche_fichier(fileFD fichier, str nom,pt_critere critere);

static void* recherche_fichier_thread(void* sarg){
	t_search_thread_args* args=sarg;
	if(!sarg)return NULL;

	return recherche_fichier(args->fd,args->filename,args->crit);
};
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/




/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							IMPLLEMENTATION FONCTIONS PUBLIQUES							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
h_hmap recherche_fichier(fileFD rootdb,str nom, pt_critere critere)
{
	unsigned long   line_number			 = 0;
	str  			ligne				 = NULL;
	size_t			taille_ligne 		 = 0;
	FILE*  			f_db = NULL;
	t_tag_fichier 	fichier_present		 = tag_fichier_ERREUR;
	h_hmap			map 				 = NULL;
	int				critere_vide	 	 = FALSE;
	int fd=-1;
	unsigned long   id_max               =0;
	unsigned long   id_last              =0;

	/* 
	 * @note	HLR15 : Verification du nom du fichier avant ouverture. Les messages 
	 *				d'erreur sont generes par la fonction verifier_fichier.
	 */
	if((fichier_present = verifier_fichier(nom) )== tag_fichier_ERREUR)
	{
		return EXIT_FAILURE;
	}

	if((fd=openat(rootdb,nom,O_RDONLY))<0){
		return EXIT_FAILURE;
	}
	if(!(f_db=fdopen(fd,"rt"))){
		return EXIT_FAILURE;
	}
	h_Buffer buf=NULL;
	BufferStream_GetLine(buf,f_db);

	// RELATES HLR05
	/*#ifdef __INTERFACE_H_DEBUG_LECTURE//---------------------------------
	int cpt=0;
	//	@note	HLR11 : La fonction parcour le fichier ligne par ligne.
	//				Scan de chaque ligne de texte du fichier jusqu'a EOF.
	//
	while(getline(&ligne, &taille_ligne, f_db) != -1)
	{
		//RELATE HLR015
		//on imprime la ligne 10 fois
		printf(ligne); 
		if(++cpt>10)
			return EXIT_SUCCESS;
		
		
		// * @note	HLR12 : La lecture des champs d'une ligne d'un fichier specifique
		// *				est geree par une fonction du module parser.h.

		// Recherche des titres
		switch(fichier_present)
		{
			// Recherche des titres.
			case tag_fichier_TITRE :
				{
					h_donnee_titre donnee =NULL;
					printf("un fichier de titre \n");
				}
				break; // FIN DU CAS TITRE.

			// Recherche des personnes
			case tag_fichier_PERSONNE :
				{
					h_donnee_nom donnee =NULL;
					printf("un fichier de noms \n");
				}
				break; // FIN DU CAS PERSONNE.

			// Recherche des cotes.
			case tag_fichier_COTE :
				{
					h_donnee_cote donnee =NULL;
					printf("un fichier de cotes \n");

				}
				break; // FIN DU CAS COTE.

			default :
				{
					// Si la ligne n'est pas libere, liberer.
					if(ligne != NULL)
					{
						free(ligne);
						ligne = NULL;
						taille_ligne = 0;
					}

					// Impossible a atteindre, mais prudent.
					printf(
						"FATAL ERROR IN : recherche_fichier() : DEFAULT CASE OCCURED.\n");
					fclose(f_db);

					return EXIT_FAILURE;

				}// FIN DU CAS PAR DEFAUT.
		}
		// Liberation de la ligne et preparation pour la prochaine lecture
		free(ligne);
		ligne = NULL;
		taille_ligne = 0;
	}
	// Si la ligne n'est pas libere, liberer.
	if(ligne != NULL)
	{
		free(ligne);
		ligne = NULL;
		taille_ligne = 0;
	}
	return EXIT_SUCCESS;

	#else*///--------------------------------------------------------------
	//on s'assure que le critere est non-nul.
	if( !critere ){
		#ifdef __DEBUG_INTERFACE_H
		printf("ERREUR critere null\n");
		#endif

		fclose(f_db);
		close(fd);
		return EXIT_FAILURE;
	}
	
	//on regarde si on a a rechercher quoique ce soit pour le fichier
	switch(fichier_present){

		case tag_fichier_PERSONNE:
			if( !critere_get_taille(tag_personne,critere)){

				#ifdef __DEBUG_INTERFACE_H
				printf("DEBUG_INTERFACE: critere_get_taille: tag_personne vide.\n");
				#endif
				critere_vide=TRUE;
			}
			break;

		case tag_fichier_TITRE:
			if(	!critere_get_taille(tag_titre,critere) ) 
			{
				#ifdef __DEBUG_INTERFACE_H
				printf("DEBUG_INTERFACE: critere_get_taille:  tag_titre/annee vide\n");
				#endif
				critere_vide=TRUE;
			}
			break;

		case tag_fichier_COTE:	
			if( !critere_get_taille(tag_cote,critere)){
				#ifdef __DEBUG_INTERFACE_H
				printf("DEBUG_INTERFACE: critere_get_taille:  cote vide\n");
				#endif
				critere_vide=TRUE;
			}
			break;

		default:{
			#ifdef __DEBUG_INTERFACE_H
			printf("ERREUR tag_fichier_erreur \n");
			#endif

			fclose(f_db);
			close(fd);
			return EXIT_FAILURE;

		}

	}
	
	//si le critere est vide, on libere le fichier et exit en succes car certains criteres 
	//ne sont pas requis
	if(critere_vide){
		fclose(f_db);
		close(fd);
		return EXIT_SUCCESS;
	}

	//sinon on cree un nouveau map pour la recherche
	if(!( map = hmap_creer())){

		#ifdef __DEBUG_INTERFACE_H
		printf("ERREUR hmap_creer\n");
		#endif
		fclose(f_db);
		close(fd);
		return EXIT_FAILURE;
	}

	//
	//	@note	HLR11 : La fonction parcour le fichier ligne par ligne.
	//				Scan de chaque ligne de texte du fichier jusqu'a EOF.
	//
	int map_detruite=FALSE; //pour break prematurement
	while(BufferStream_GetLine(buf,f_db)>0){	
		
		if(map_detruite) 
			break;

		// Recherche differente pour chaque fichier.
		switch(fichier_present)
		{
			// Recherche des titres.
			case tag_fichier_TITRE :
				{
					
					h_donnee_titre donnee =NULL;
					//parse la ligne en la comparant au critere
					if( ( donnee = parse_ligne_titre_et_critere(buf->buf_head,critere) ) ){
						/**
						* @note   sx-HLR04
						*/
						donnee_titre_set_num_ligne(donnee,line_number);

						#ifdef __DEBUG_INTERFACE_H
						printf("TRACE tag_titre:donnee non nule par parse \n");
						#endif

						//si on a un succes on ajoute au hashmap
						if( hmap_add(	
								map									,
								donnee_titre_get_id_titre(donnee)	,
								(void*) donnee						,
								&donnee_titre_liberer				
							)  == EXIT_FAILURE )
						{
							#ifdef __DEBUG_INTERFACE_H
							printf("ERREUR:recherche_fichier: hmap_add failed titre \n");
							#endif

							hmap_liberer(map);
							map=NULL;
							map_detruite=TRUE;
						}
					}
				}
				break; // FIN DU CAS TITRE.
				
			// Recherche des personnes
			case tag_fichier_PERSONNE :
				{
					h_donnee_nom donnee =NULL;
					//parse la ligne en la comparant au critere
					if( donnee = parse_ligne_nom_et_critere(ligne,critere) ){
						/**
						* @note   sx-HLR04
						*/
						donnee_nom_set_num_ligne(donnee,line_number);
						#ifdef __DEBUG_INTERFACE_H
						printf("TRACE tag_nom:donnee non nule par parse \n");
						#endif


						//si on a un succes on ajoute au hashmap
						if( hmap_add(	
								map								,
								donnee_nom_get_id_nom(donnee)	,
								(void*) donnee					,
								&donnee_nom_liberer				
							)  == EXIT_FAILURE )						
						{
							#ifdef __DEBUG_INTERFACE_H
							printf("ERREUR:recherche_fichier: hmap_add failed nom \n");
							#endif

							hmap_liberer(map);
							map=NULL;
							map_detruite=TRUE;
						}
					}
				}
				break; // FIN DU CAS PERSONNE.
				
			// Recherche des cotes.
			case tag_fichier_COTE :
				{
					h_donnee_cote donnee =NULL;

					//parse la ligne en la comparant au critere
					if( donnee = parse_ligne_cote_et_critere(ligne,critere) ){
						/**
						* @note   sx-HLR04
						*/
						donnee_cote_set_num_ligne(donnee,line_number);

						#ifdef __DEBUG_INTERFACE_H
						printf("TRACE tag_cote:donnee non nule par parse \n");
						#endif

						//si on a un succes on ajoute au hashmap
						if( hmap_add(	
								map								,
								donnee_cote_get_id_titre(donnee)	,
								(void*) donnee					,
								&donnee_cote_liberer				
							)  == EXIT_FAILURE )
						{
							#ifdef __DEBUG_INTERFACE_H
							printf("ERREUR:recherche_fichier: hmap_add failed cote \n");
							#endif


							//erreur fatale au niveau du stockage
							hmap_liberer(map);
							map=NULL;
							map_detruite=TRUE;
						}
					}
				}
				break; // FIN DU CAS COTE.
				

			default :
				{
					// Si la ligne n'est pas libere, liberer.
					if(ligne != NULL)
					{
						free(ligne);
						ligne = NULL;
						taille_ligne = 0;
					}
					// Impossible a atteindre, mais prudent.
					printf(
						"FATAL ERROR IN : recherche_fichier() : DEFAULT CASE OCCURED.\n");
					fclose(f_db);

					//on supprime la map
					hmap_liberer(map);
					map=NULL;
					map_detruite=TRUE;
				}// FIN DU CAS PAR DEFAUT.
		}

		// Liberation de la ligne et preparation pour la prochaine lecture
		BufferMove(buf);
		++line_number;
	}


	// Si la ligne n'est pas libere, liberer.
	if(buf->buf_head != NULL)
	{
		BufferFree(buf);
	}

	//on regarde si la map est nonnule, erreur sinon
	if(map_detruite){
		fclose(f_db);
		close(fd);
		return EXIT_FAILURE;
	}
	//#endif//-------------------------------------------------------------


	/*
	 * @note	HLR13 : Tout fichier est fermee a la fin de l'utilisation.
	 */
	fclose(f_db);
	close(fd);
	#ifdef __DEBUG_INTERFACE_H
	printf("TRACE:recherche_fichier: SUCCESS\n\n");
	#endif

	return map;
}

t_resultats_recherche* recherche_simult(fileFD dbroot,pt_critere crit){
	t_search_thread_args arg_nom,arg_titre,arg_cote;
	t_resultats_recherche* result=NULL;
	pthread_t tid_nom,tid_cote,tid_titre;
	fileFD fdt=openat(dbroot,FICHIER_TITRE);

	if(!crit)
		return NULL;
	

	if(!(result=malloc(sizeof(t_resultats_recherche))))
		return NULL;

	arg_titre.fd=dbroot;arg_nom.fd=dbroot;arg_cote.fd=dbroot;
	arg_titre.filename=FICHIER_TITRE;
	arg_nom.filename=FICHIER_NOMS;
	arg_cote.filename=FICHIER_COTE;
	arg_titre.crit=crit;
	arg_nom.crit=crit;
	arg_cote.crit=crit;

	pthread_create(&tid_titre,NULL,recherche_fichier_thread,&arg_titre);
	pthread_create(&tid_nom,NULL,recherche_fichier_thread,&arg_nom);
	pthread_create(&tid_cote,NULL,recherche_fichier_thread,&arg_cote);

	pthread_join(tid_titre,&result->map_titre);
	pthread_join(tid_nom,&result->map_nom);
	pthread_join(tid_cote,&result->map_cote);
	
	result->map_union=analyser_resultats(
		result->map_titre,
		result->map_nom,
		result->map_cote,
		crit
	);

	return result;
}

/**
 * @note   Sx:HLR-08
 * @note   Sx:HLR-09
 * @note   Sx:HLR-10
 * @note   Sx:HLR-11
 * @note   Sx:HLR-18
 */
int op_supprimer(fileFD dbroot,pt_critere crit){
	t_resultats_recherche* 	result=NULL;
	t_op_supprimer_args*	args=NULL;
	int fdin=-1;
	int fdout=-1;
	FILE* f_db=NULL;
	FILE* f_tmp=NULL;
	str ligne_in=NULL;
	char name_out[128];
	int i=2;
	int ligne_in_sz=0;
	int retval;

	if(!crit)									return -1;
	if(!(result=recherche_simult(dbroot,crit)))	return -1;
	
	//le comportement de pattern-match demande dans l'enonce est le meme que pour 
	//l'operation de recherche. si c'est pas ce que vous vouliez, vouz deviez etre plus
	//clair

	if(result->map_cote) 	hmap_liberer(result->map_cote);
	if(result->map_titre) 	hmap_liberer(result->map_titre);
	if(result->map_nom)		hmap_liberer(result->map_nom);
	result->map_nom=NULL;
	result->map_titre=NULL;
	result->map_cote=NULL;
	str dbfiles[3]={FICHIER_TITRE,FICHIER_COTE,FICHIER_NOMS};

	if(	!result->map_union )	return -1;

	if(!(args=op_supprimer_preproc(result->map_union))){
		hmap_liberer(result->map_union);
		result->map_union=NULL;
		return -1;
	}
	
	_file_open:{
		if((fdin=openat(dbroot,dbfiles[i],O_RDONLY))<0){
			list_free(args->list_cote);
			list_free(args->list_nom);
			list_free(args->list_titre);
			list_free(args->list_id_titre);
			free(args);
			hmap_liberer(result->map_union);
		}

		//get a name for output
		
		memcpy(name_out,dbfiles[i],strlen(db_files[i])+1);
		name_out[strlen(name_out)-1]++;

		//open output
		if((fdout=openat(dbroot,name_out,O_WRONLY|O_CREAT))<0){
			close(fdin);
			retval=-1;
			goto _end;
		}
		//open 
		if(!(f_db=fdopen(fdin,"rt")) || !(f_tmp=fdopen(fdout,"wt")) ){
			fclose(f_db);
			fclose(f_tmp);
			close(fdin);
			close(fdout);
			retval=-1;
			goto _end;
		}
		switch(i){
			case 0: goto _file_name;
			case 1: goto _file_title;
			case 2: goto _file_cote;
			default: goto _file_name;
		}
	}
	
	_file_cote:{
		unsigned long numline=0;
		h_donnee_resultat cur=list_pop_head(args->list_cote);

		while(getline(&ligne_in,&ligne_in_sz,f_db)!=-1){
			if(cur && donnee_resultat_get_num_cote(cur)==numline){
				++numline;
				continue;//skip
			}

			//else print
			fprintf(f_tmp,"%s\n",ligne_in);
			++numline;
		}
		goto _file_close;
	}

	_file_name:{
		while(getline(&ligne_in, &ligne_in_sz, f_db) != -1){
			//on formate
			str line=ligne_formater(ligne_in);					

			//pour toute les lignes a suprimer
			for(	str p=list_itor_head(args->list_id_titre);
					p;
					p=list_itor_next(args->list_id_titre)
			){
				//on cree la string formatee du id_titre avec virgule en avant
				str buf=fstr_remplacer_make(",dest","dest",p);
				
				//on remplace les correspondances dans la ligne dorigine
				fstr_remplacer_dest(line,&line,buf,"");

				//on cree la string formatee du id_titre avec la virgule apres
				fstr_remplacer_dest("dest,",&buf,"dest",p);

				//on en remplace les instances dans la ligne
				fstr_remplacer_dest(line,&line,buf,"");

				//on remplace sil en rest sans virgule
				fstr_remplacer_dest(line,&line,p,"");
			}
			//si le nom a au moins un titre on lecrit, sinon il est supprimer
			if(!strstr(line,"\t\t"))
				fprintf(f_tmp,"%s\n",line);
			free(line);
		}
		goto _file_close;
	}

	_file_title:{
		unsigned long numline=0;
		h_donnee_resultat cur=list_pop_head(args->list_titre);

		while(getline(&ligne_in,&ligne_in_sz,f_db)!=-1){
			if(cur && donnee_resultat_get_num_cote(cur)==numline){
				++numline;
				continue;//skip
			}

			//else print
			fprintf(f_tmp,"%s\n",ligne_in);
			++numline;
		}
		goto _file_close;
	}

	_file_close:{
		fsync(fdout);
		fclose(f_tmp);
		close(fdout);
		fclose(f_db);
		close(fdin);
		//delete old
		unlinkat(
			pfs->db_files[i]->parent->fd,
			pfs->db_files[i]->name,
			0
		);
		//rename new
		renameat(
			pfs->data_store->fd,
			name_out,
			pfs->data_store->fd,
			pfs->db_files[i]->name
		);
		//open new
		pfs->db_files[i]->fd=openat(
			pfs->data_store->fd,
			pfs->db_files[i]->name,
			O_RDWR|O_NONBLOCK
		);
		if(i){
			//setup for next file
			memset(name_out,0,strlen(name_out));
			--i;
			goto _file_open;
		}
		//success
		retval=0;
		goto _end;
	}

	_end:{
		list_free(args->list_cote);
		list_free(args->list_id_titre);
		list_free(args->list_nom);
		list_free(args->list_titre);
		free(args);
		hmap_liberer(result->map_union);
		result->map_union=NULL;
		return retval;
	}

}

int op_ajouter(h_private_file_system pfs, pt_critere crit){
	t_resultats_recherche* 	result=NULL;
	t_op_supprimer_args*	args=NULL;
	h_list					list_nomatch=NULL;
	int fdin=-1;
	int fdout=-1;
	FILE* f_db=NULL;
	FILE* f_tmp=NULL;
	str ligne_in=NULL;
	char name_out[128];
	int i=0;
	int ligne_in_sz=0;
	int retval;
	char id_titre[64];memset(id_titre,0,64);

	return -1;//pas eu le temps de finir..


	if(!pfs||!crit)								return -1;
	if(critere_get_taille(tag_titre,crit)!=1)	return -1;
	if(!(result=recherche_simult(pfs,crit)))	return -1;

	if(		!(list_nomatch=list_new(NULL,NULL))	
		||	!(args=op_supprimer_preproc(result->map_union))
	){
		list_free(list_nomatch);
		hmap_liberer(result->map_union);result->map_union=NULL;
		hmap_liberer(result->map_cote);	result->map_cote=NULL;
		hmap_liberer(result->map_titre);result->map_titre=NULL;
		hmap_liberer(result->map_nom);	result->map_nom=NULL;
		free(result);
		return -1;
	}

	//build the name insertion list---------------------------------
	int elem=0;
	int listelem=0;
	str notthere=NULL;
	if( list_get_nb_elem(args->list_nom)<(elem=critere_get_taille(tag_personne,crit))){
		for(int nm=elem-1;nm>=0;--nm){
			BOOL match=FALSE;
			for(h_donnee_nom n=list_itor_head(args->list_nom);
				n;
				n=list_itor_next(args->list_nom)
			){
				match|=(strcmp(
					donnee_nom_get_nom(list_get_elem_index(args->list_nom,listelem)),
					(notthere=critere_get_element_i(crit,tag_personne,nm))
				)==0);
			}
			if(!match)
				list_push_head(list_nomatch,notthere);
		}
	}

	//check if the title is an insertion--------------------------------------
	if(list_get_nb_elem(args->list_titre)){
		h_donnee_titre tmp=list_get_elem_index(args->list_titre,0);
		strcpy(id_titre,donnee_titre_get_id_titre(tmp));
	}


	_file_open:{
		if((fdin=openat(
			pfs->db_files[i]->parent->fd,
			pfs->db_files[i]->name,
			O_RDONLY
		))<0){
			retval=-1;
			goto _end;
		}

		//get a name for output
		memcpy(name_out,pfs->db_files[i]->name,strlen(pfs->db_files[i]->name)+1);
		name_out[strlen(name_out)-1]++;

		//open output
		if((fdout=openat(
			pfs->db_files[i]->parent->fd,
			name_out,
			O_WRONLY|O_CREAT
		))<0){
			close(fdin);
			retval=-1;
			goto _end;
		}
		//open 
		if(!(f_db=fdopen(fdin,"rt")) || !(f_tmp=fdopen(fdout,"wt")) ){
			fclose(f_db);
			fclose(f_tmp);
			close(fdin);
			close(fdout);
			retval=-1;
			goto _end;
		}
		switch(i){
			case 0: goto _file_name;
			case 1: goto _file_title;
			case 2: goto _file_cote;
			default: goto _file_name;
		}
	}

	_file_title:{
		
	}

	_file_close:{
		fsync(fdout);
		fclose(f_tmp);
		close(fdout);
		fclose(f_db);
		close(fdin);
		//delete old
		unlinkat(
			pfs->db_files[i]->parent->fd,
			pfs->db_files[i]->name,
			0
		);
		//rename new
		renameat(
			pfs->data_store->fd,
			name_out,
			pfs->data_store->fd,
			pfs->db_files[i]->name
		);
		//open new
		pfs->db_files[i]->fd=openat(
			pfs->data_store->fd,
			pfs->db_files[i]->name,
			O_RDWR|O_NONBLOCK
		);
		if(i){
			//setup for next file
			memset(name_out,0,strlen(name_out));
			--i;
			goto _file_open;
		}
		//success
		retval=0;
		goto _end;
	}

	_end:{
		list_free(args->list_cote);
		list_free(args->list_id_titre);
		list_free(args->list_nom);
		list_free(args->list_titre);

		free(args);
		hmap_liberer(result->map_union);
		result->map_union=NULL;
		return retval;
	}


}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
h_hmap analyser_resultats(h_hmap titre,h_hmap nom,h_hmap cote,pt_critere crit){	
	h_hmap 			resultats=NULL;
	h_table_string 	table_id_titres;
	h_list			ls=NULL;

	//on verifie que le critere na pas change d'adresse
	//validation des entrees. certains hmap peuvent etre null si le critere le justifie
	if(	!crit 	 || !titre ||
	    ( !nom  && critere_get_taille(tag_personne, crit) )||
		( !cote && critere_get_taille(tag_cote,     crit) )
	){
		//soit les donnees ne permettent pas de se concorder au 
		return NULL;
	}


	//cree la map des resultats
	if(! ( resultats=hmap_creer() )){
		return NULL;
	}

	//on ajoute tous les titres trouves au hashmap, retourne si erreur
	//si le map est nul, on se doit dechouer
	if(	EXIT_FAILURE==hmap_iterer_paires(
			titre,
			ajouter_resultat_titre_hmap,
			(void*)resultats
	)){
		#ifdef __TRACE_ANALYSE_RESULTATS
		printf("TRANSFER OF m_resultat.titre TO RESULTS FAILED.");
		#endif

		hmap_liberer(resultats);

		return NULL;
	}

	//on ajoute tous les noms qui trouvent leur titre dans le hmap au hmap
	//vue que cet argument est optionnel le map pourait etre nul
	if(nom){
		if(	EXIT_FAILURE==hmap_iterer_paires(	
				nom,
				ajouter_resultat_nom_hmap,
				(void*)resultats
		)){
			#ifdef __TRACE_ANALYSE_RESULTATS
			printf("UNION OF m_resultat.nom TO RESULTS FAILED.\n");
			#endif

			hmap_liberer(resultats);
			return NULL;
		}
	}

	//on ajoute toutes les cotes qui trouvent leur titre dans le hmap
	//vue que cet argument est optionnel le map pourait etre nul
	if(cote){
		if(	EXIT_FAILURE==hmap_iterer_paires(
				cote,
				ajouter_resultat_cote_hmap,
				(void*)resultats
		)){
			#ifdef __TRACE_ANALYSE_RESULTATS
			printf("UNION OF m_resultat.cote TO RESULTS FAILED.\n");
			#endif

			hmap_liberer(resultats);
			return NULL;
		}
	}


	//on clean le map de resultats en validant quils ont tous les elements necessaires
	if(	EXIT_FAILURE==hmap_iterer_paires_supprimer_si( 
			resultats,
			si_resultat_invalide_success,
			crit
	)){
		hmap_liberer(resultats);
		return NULL;
	}
	if(	EXIT_FAILURE==hmap_iterer_paires( 
			resultats,
			&ajouter_resultat_lignes,
			ls
	)){
		hmap_liberer(resultats);
		return NULL;
	}

	//on s'assure de supprimer toute ancienne recherche

	return resultats;
}
//----------------------------------------------------------------------------------------
t_tag_fichier verifier_fichier(str nom_fichier){
	// Verification de l'existence du parametre.
	if(nom_fichier == NULL)
	{
		printf("ERROR, UNINITIALIZED FILE NAME.\n");
		return tag_fichier_ERREUR;
	}
	// Allocation dynamique d'une variables tampon de la meme dimension que l'entree.
	//str copie_fichier = (str) malloc((strlen(nom_fichier)+1)*sizeof(char));

	// Variables pour contenir les chaines entre les separateurs.
	//str token;
	//str last_token;

	// Copie de l'entree pour ne pas l'affecter par le strtok.
	//memcpy(copie_fichier, nom_fichier, (strlen(nom_fichier)+1)*sizeof(char));
	h_table_string table=NULL;
	if(!(table=table_string_creer(nom_fichier,"/")))
	{
		return tag_fichier_ERREUR;
	}


	//on get le dernier token
	unsigned int nb=table_string_get_taille(table);
	str last_token=NULL;

	if(!(last_token=table_string_get_element_n(table,nb-1))){
		table_string_liberer(table);
		return tag_fichier_ERREUR;
	}

	// Verification du nom du fichier avec les references.
	if(!strcmp(last_token, FICHIER_TITRE))
	{
		table_string_liberer(table);
		return tag_fichier_TITRE;
	}
	else if(!strcmp(last_token, FICHIER_NOMS))
	{
		table_string_liberer(table);
		return tag_fichier_PERSONNE;
	}
	else if(!strcmp(last_token, FICHIER_COTE))
	{
		table_string_liberer(table);
		return tag_fichier_COTE;
	}
	else
	{
		// Message d'erreur en cas de nom invalide.
		printf("ERROR, UNKNOWN DATA FILE NAME : %s.\n", nom_fichier);
		return tag_fichier_ERREUR;
	}
}
//----------------------------------------------------------------------------------------
int imprimer_resultat_nom(cstr const nom_fichier,h_hmap nom){
	FILE* fp=NULL;

	//on ouvre le fichier de sortie tout en verifiant si le resultat existe
	if(!nom|| !( fp = fopen(nom_fichier, "wt") ) ){
		fp=NULL;
		return EXIT_FAILURE;
	}

	//on itere sur toutes les paires en imprimant
	if(	EXIT_FAILURE==hmap_iterer_paires(
			nom,
			&donnee_afficher_nom, 
			(void*)fp
	)){
		fclose(fp);
		return EXIT_FAILURE;
	}

	fclose(fp);
	fp=NULL;
	return EXIT_SUCCESS;
}
//----------------------------------------------------------------------------------------
int imprimer_resultat_cote(cstr const nom_fichier,h_hmap cote){
	FILE* fp=NULL;

	//on ouvre le fichier de sortie tout en verifiant si le resultat existe
	if(! cote || !( fp = fopen(nom_fichier, "wt") ) ){
		fp=NULL;
		return EXIT_FAILURE;
	}

	if(	EXIT_FAILURE==hmap_iterer_paires(
			cote,
			&donnee_afficher_cote, 
			(void*)fp
	)){
		fclose(fp);
		return EXIT_FAILURE;
	}

	fclose(fp);
	fp=NULL;
	return EXIT_SUCCESS;
}
//----------------------------------------------------------------------------------------
int imprimer_resultat_titre(cstr const nom_fichier,h_hmap titre){
	FILE* fp=NULL;

	//on ouvre le fichier de sortie tout en verifiant si le resultat existe
	if(!titre || !( fp = fopen(nom_fichier, "wt") ) ){
		fp=NULL;
		return EXIT_FAILURE;
	}

	//on itere sur toutes les paires en imprimant
	if(	EXIT_FAILURE==hmap_iterer_paires(
			titre,
			&donnee_afficher_titre, 
			(void*)fp
	)){
		fclose(fp);
		return EXIT_FAILURE;
	}

	fclose(fp);
	fp=NULL;

	return EXIT_SUCCESS;
}
//----------------------------------------------------------------------------------------
int imprimer_resultat_final(cstr const nom_fichier,h_hmap resultat){
	FILE* fp=NULL;

	//on ouvre le fichier de sortie tout en verifiant si le resultat existe
	if( !resultat || !( fp = fopen(nom_fichier, "wt" ) ) ){
		return EXIT_FAILURE;
	}

	//on itere sur toutes les paires en imprimant
	if(	EXIT_FAILURE==hmap_iterer_paires(
			resultat,
			&donnee_afficher_resultat, 
			(void*)fp
	)){
		fclose(fp);
		return EXIT_FAILURE;
	}

	fclose(fp);
	return EXIT_SUCCESS;
}
//----------------------------------------------------------------------------------------



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*					IMPLEMENTATION FONCTIONS PRIVEES           							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

t_op_supprimer_args* op_supprimer_preproc(h_hmap result){
	t_op_supprimer_args* nouv=NULL;

	if(!result)
		return NULL;

	if(!(nouv=malloc(sizeof(t_op_supprimer_args)))){
		return NULL;
	}
	if(!(nouv->list_cote=list_new(NULL,donnee_resultat_ligne_cote_leq))){
		free(nouv);
		return NULL;
	}
	if(!(nouv->list_titre=list_new(NULL,donnee_resultat_ligne_titre_leq))){
		free(nouv->list_cote);
		free(nouv);
		return NULL;
	}
	if(!(nouv->list_nom=list_new(NULL,donnee_resultat_ligne_nom_leq))){
		free(nouv->list_cote);
		free(nouv->list_titre);
		free(nouv);
		return NULL;
	}
	if(!(nouv->list_id_titre=list_new(free,NULL))){
		free(nouv->list_cote);
		free(nouv->list_titre);
		free(nouv->list_nom);
		free(nouv);
		return NULL;
	}
	hmap_to_list(result,nouv->list_cote,FALSE);
	hmap_to_list(result,nouv->list_titre,FALSE);
	hmap_to_list(result,nouv->list_nom,FALSE);
	hmap_iterer_paires(result,ajouter_resultat_id_titre_list,nouv->list_id_titre);

	return nouv;
}

extern void ajouter_resultat_id_titre_list(void* contexte, str   cle, void* valeur){
	h_list ls=contexte;
	str dupcle;
	if(!contexte||!cle||!valeur)
		return;

	if(!(dupcle=strdup(cle))) return;
	if(valeur) list_push_head(ls,dupcle);
}
extern void ajouter_resultat_titre_hmap(
	void* 				contexte, 
	cstr 	cle, 
	void* 				valeur
){

	h_donnee_titre 		donnee_titre 	=(h_donnee_titre) valeur;
	h_hmap 				destination  	=contexte;
	h_donnee_resultat 	resultat		=NULL;
	unsigned int        nouveau			=0;

	if(!contexte || !cle ||!valeur)
		return;

	//on regarde si on l'a deja
	if(!( resultat=(h_donnee_resultat)hmap_get(destination,cle)) ){
		
		// si non on cree un nouveau
		if(! (resultat=donnee_resultat_creer()))
			return;

		nouveau=1;
	}

	//on transfert le data
	donnee_resultat_set_id_titre(resultat,donnee_titre_get_id_titre(donnee_titre));
	donnee_resultat_set_titre(resultat,donnee_titre_get_titre(donnee_titre));
	donnee_resultat_set_annee(resultat,donnee_titre_get_annee(donnee_titre));
	donnee_resultat_set_ligne_titre(resultat,donnee_titre_get_ligne(donnee_titre));
	donnee_resultat_set_num_ligne(resultat,
		donnee_titre_get_num_ligne_titre(donnee_titre));

	//si nouveau on doit ajouter au map
	if(nouveau)
		hmap_remplacer(destination,cle,resultat,&donnee_resultat_liberer);
	
}
//----------------------------------------------------------------------------------------
extern void ajouter_resultat_nom_hmap(
	void* 				contexte, 
	cstr const 	cle, 
	void* 				valeur
){
	h_hmap 				map				=(h_hmap)contexte;
	h_donnee_nom 		data_nom		=(h_donnee_nom)valeur;
	h_donnee_resultat	destination		=NULL;
	h_table_string  	titre_ids		=NULL;
	int					taille_table	=0;

	if( !contexte || ! cle || !valeur )
		return;
	
	//on obtient le tuple de id de titre et on le tokenize
	if( !(	titre_ids=table_string_creer(
				donnee_nom_get_id_titres(data_nom),
				","
		)	) || !(taille_table=table_string_get_taille(titre_ids))			
	){
		table_string_liberer(titre_ids);
		return;
	}
	

	//pour chaque token obtenu, si il est dans les resultats, on transfere le data
	for(int i=0;i<taille_table;++i){

		if( destination=(h_donnee_resultat)hmap_get( 
				map, 
				table_string_get_element_n(titre_ids,i)
			)
		){
			donnee_resultat_set_nom(destination,donnee_nom_get_nom(data_nom));
			donnee_resultat_set_num_ligne_nom(destination,
				donnee_nom_get_num_ligne(data_nom));
		}
	}
	table_string_liberer(titre_ids);
}
//----------------------------------------------------------------------------------------
extern void ajouter_resultat_cote_hmap(
	void* 				contexte, 
	cstr const 	cle, 
	void* 				valeur
){
	h_hmap 				map				=(h_hmap)contexte;
	h_donnee_cote 		data_cote		=(h_donnee_cote)valeur;
	h_donnee_resultat	destination		=NULL;

	if( !contexte || ! cle || !valeur )
		return;

	str id=donnee_cote_get_id_titre(data_cote);
	str cote=donnee_cote_get_cote(data_cote);

	destination=(h_donnee_resultat)hmap_get(map,id);

	//on obtient le node du hmap si il existe et on transfert le data
	if(destination){
		donnee_resultat_set_cote(destination,cote);
		donnee_resultat_set_num_ligne_cote(destination,
			donnee_cote_get_num_ligne(data_cote));
	}
}
extern void ajouter_resultat_lignes(
	void* 				contexte, 
	cstr const 	cle, 
	void* 				valeur
){
	h_resultat_ligne 	ls=(h_resultat_ligne)contexte;

	if( !contexte || ! cle || !valeur )
		return;
	h_list cotes=resultat_ligne_get_liste_cote(ls);
	h_list titres=resultat_ligne_get_liste_titre(ls);
	h_list noms=resultat_ligne_get_liste_nom(ls);
	list_push_sort(cotes,strdup(cle));
	list_push_sort(noms,strdup(cle));
	list_push_sort(titres,strdup(cle));
}
//----------------------------------------------------------------------------------------
extern int si_resultat_invalide_success(
	void* 				contexte, 
	cstr const 	cle, 
	void* 				valeur
){
	pt_critere critere			=(pt_critere)contexte;
	h_donnee_resultat resultat	=(h_donnee_resultat)valeur;
	
	//pour clarete
	int invalide=EXIT_SUCCESS;
	int valide=EXIT_FAILURE;

	//on verifie les pointeurs
	if(!contexte || !cle || !valeur)
		return invalide;
	

	#ifdef __TRACE_VERIFIER_RESULTAT

	//dump console du resultat sous analyse et de la decision
	str cote=donnee_resultat_get_cote( resultat);
	str titre=donnee_resultat_get_titre(resultat);
	str nom=donnee_resultat_get_nom(  resultat);
	str annee=donnee_resultat_get_annee(resultat);
	str ligne=donnee_resultat_get_ligne_titre(resultat);

	int tc=critere_get_taille(tag_cote,    critere);
	int tt=critere_get_taille(tag_titre,   critere);
	int tn=critere_get_taille(tag_personne,critere);
	int ta=critere_get_taille(tag_annee,	critere);

	printf("HMAP[%s]{\n",cle);
	printf("\tCote:%s -> %d\n",cote,tc);
	printf("\tTitre:%s -> %d\n",titre,tt);
	printf("\tpersonne:%s -> %d\n",nom,tn);
	printf("\tannee:%s -> %d\n",annee,ta);
	printf("\tligne:%s -> %d\n}\t->",ligne);

	if( (tc&&!cote) || (tt&&!titre) || (tn&&!nom) || (ta&&!annee) ){
		printf("DISCARDED\n\n");
		return invalide;
	}

	printf("KEPT\n\n");
	return valide;

	#else

	//si un type de critere manque a lappel on retourne echec
	if((critere_get_taille(tag_cote,    critere)&& !donnee_resultat_get_cote( resultat))||
	   (critere_get_taille(tag_titre,   critere)&& !donnee_resultat_get_titre(resultat))||
	   (critere_get_taille(tag_personne,critere)&& !donnee_resultat_get_nom(  resultat))||
	   (critere_get_taille(tag_annee,	critere)&& !donnee_resultat_get_annee(resultat))
	){
		return invalide;
	}
	return valide;//sinon valide
	#endif;					
}
//----------------------------------------------------------------------------------------
/*
BOOL  liste_id_titre_add_id_disponible(str id_disponible){
	if(!m_id_titre_disponible){
		if(!( m_id_titre_disponible=list_new(free,num_ligne_leq) ) )
			return FALSE;
	}
	return (
		id_disponible
		?	list_push_sort(m_id_titre_disponible,id_disponible)
		:	FALSE
	);
}
//----------------------------------------------------------------------------------------
str liste_id_titre_get_id_disponible(){
	static str  m_dernier_id_titre	=NULL;

	if(!m_id_titre_disponible){
		if(!( m_id_titre_disponible=list_new(free,num_ligne_leq) ) )
			return NULL;
	}
	if(!list_get_nb_elem(m_id_titre_disponible))
		return NULL;

	if(m_dernier_id_titre)
		free(m_dernier_id_titre);

	m_dernier_id_titre=list_pop_head(m_id_titre_disponible);

	//on cree le nouveau tag au besoin
	if(!list_get_nb_elem(m_id_titre_disponible)){
		str buf=strdup(m_dernier_id_titre);		//on copie
		int   len=strlen(buf+2);	//la taille du buffer

		memset(buf+2,'0',len);		//copie

		str num=strdup(m_dernier_id_titre);				//on copie le tag
		sprintf(num,"%ll",atoll(num+2)+1);		//on y ecrit le id manquant
		sprintf(buf+2+len-strlen(num),"%s",num);//on copie le resultat dans le buffer
		free(num);								//on libere le buffer du chiffre
		list_push_head(m_id_titre_disponible,buf);	//on ajoute le id aux disponibles
						
	}
	return strdup(m_dernier_id_titre);
}
//----------------------------------------------------------------------------------------
str liste_id_nom_get_id_disponible(){
	static str  m_dernier_id_nom		=NULL;

	if(!m_id_nom_disponible){
		if(!( m_id_nom_disponible=list_new(free,num_ligne_leq) ) )
			return NULL;
	}
	if(!list_get_nb_elem(m_id_nom_disponible))
		return NULL;

	if(m_dernier_id_nom)
		free(m_dernier_id_nom);

	m_dernier_id_nom=list_pop_head(m_id_nom_disponible);

	//on cree le nouveau tag au besoin
	if(!list_get_nb_elem(m_id_nom_disponible)){
		str buf=strdup(m_dernier_id_nom);		//on copie
		int   len=strlen(buf+2);	//la taille du buffer

		memset(buf+2,'0',len);		//copie

		str num=strdup(m_dernier_id_nom);				//on copie le tag
		sprintf(num,"%ll",atoll(num+2)+1);		//on y ecrit le id manquant
		sprintf(buf+2+len-strlen(num),"%s",num);//on copie le resultat dans le buffer
		free(num);								//on libere le buffer du chiffre
		list_push_head(m_id_nom_disponible,buf);	//on ajoute le id aux disponibles
						
	}
	return strdup(m_dernier_id_nom);
}
//----------------------------------------------------------------------------------------
BOOL liste_id_nom_add_id_disponible(str id_disponible){
	if(!m_id_nom_disponible){
		if(!( m_id_nom_disponible=list_new(free,num_ligne_leq)))
			return FALSE;
	}
	if(!id_disponible)
		return FALSE;

	return list_push_sort(m_id_nom_disponible,id_disponible);
}
*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
