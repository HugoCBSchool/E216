/*
	Projet		: ELE216 - Laboratoire 1
	Date		: 14 mars 2021
	Par 		: Gabriel Gandubert et Hugo Cusson-Bouthillier
	Logiciel	: Sublime text & Eclipse
	Definition	: Encapsulation des criteres de recherche pour la base de donnee de films

	Contient:
		Structure des criteres qui permet de separer les types de criteres (commandes)
		La structure est defini ici, mais le "typedef" est dans le module critere.h.

		FONCTIONS PUBLIQUES:
			unsigned int 	tag_critere_valide(t_tag tag)	--validatrice de t_tag.

			pt_critere 		critere_creer(void)				--constructrice de t_critere.
			void 			critere_liberer(pt_critere crit)--destructrice de t_critere.
			
			unsigned int    set_critere(					--mutatrice de t_critere.
				cstr 		s_tuple_critere,
				t_tag				tag,
				const pt_critere	critere 
			)												

			unsigned int critere_get_taille( 				--Informatrice nb critere.
				t_tag_critere		tag, 
				pt_critere const critere
			)

			cstr critere_get_type_titre(					--Lecture du type de titre.
				pt_critere const critere
			);

			BOOL critere_set_type_titre(					--mutateur de type_titre.
				pt_critere const critere, 
				cstr type_titre
			)

			cstr critere_get_element_i(						--Informatrice indicielle
				pt_critere 	const   critere	, 
				t_tag_critere 		tag		, 
				unsigned int 		i		
			)

			unsigned int 	critere_foreach(				--iteratrice fonctionnelle de.
				const pt_critere	critere,					t_critere
				t_tag 				tag,
				void 				(*pf)(cstr,void*), 	
				const void 			*pf_data
			)

			unsigned int critere_contient(					--Recherche dans un critere.
				pt_critere const        critere		, 
				t_tag_critere 			tag			,
				const fp_comp			f_comparer	,
				cstr					f_str2		
			)

			str critere_serialiser(							--Serialisation des criteres.
				pt_critere const 		critere		, 
				cstr					sep
			)

		FONCTIONS PRIVEEs :
			h_table_string 	critere_get_table_string(		--Pointeur d'un critere.
				pt_critere const		critere		,
				t_tag_critere			tag
			)
			
			-- Pointeur double d'un critere
			h_table_string* critere_select_table_string_handle(
				pt_critere 	const 		critere		,
				t_tag_critere 			tag
			)

			t_tag_critere critere_prochain_tag_boucle(		--Prochain tag en boucle.
				t_tag_critere 			tag
			)



*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Header file
#include "critere.h"


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							CONSTANTES D'IMPLEMENTATION     							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
// Caratere de separation des criteres.
#define 	C_CRITERE_TOKEN_C ','
/* 
 * Caratere de separation des criteres en format de chaine de caractere 
 * util pour des fonctions prenant uniquement des chaines de caractere.
 */
#define 	S_CRITERE_TOKEN_C ","
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/




/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							TYPES ET STRUCTURES OPAQUES     							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @note HLR03	: 	Structure de donnees permettant de contenir tous les arguments recu
 * 			  		dans le programme principal.
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/




/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							DECLARATIONS FONCTIONS PRIVEES    							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief  	permet d'obtenir une reference sur la table de string du critere 
 * 			voulu de la structure de critere
 * @param  	critere:	la structure de critere
 * @param  	tag: 		le tag du critere voulu
 * @retval 	succes:		un pointeur sur la table de string du critere
 * 			echec:		NULL
 */
h_table_string critere_get_table_string(pt_critere 	const critere,t_tag_critere tag);

/**
 * @brief  	permet d'obtenir une reference sur le handle de la table de string du critere 
 * 			voulu de la structure de critere
 * @param  	critere:	la structure de critere
 * @param  	tag: 		le tag du critere voulu
 * @retval 	succes:		un pointeur sur la table de string du critere
 * 			echec:		NULL
 */
h_table_string* critere_select_table_string_handle(	pt_critere 	const critere	,
													t_tag_critere 	tag			);

/**
 * @brief  	permet d'obtenir un autre tag de critere. Utile pour les boucles utilisant
 			tous les criteres.
 * @param  	tag: 		le tag du critere present
 * @retval 	succes:		donne le prochain tag basee sur le tag present.
 * 			echec:		retourne le tag des titres par defaut.
 */
t_tag_critere critere_prochain_tag_boucle(t_tag_critere tag);
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/




/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							IMPLLEMENTATION FONCTIONS PUBLIQUES							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
unsigned int tag_critere_valide(char tag){
	return	(	(tag==CHAR_TAG_ANNEE	)||
				(tag==CHAR_TAG_COTE		)||
				(tag==CHAR_TAG_PERSONNE	)||
				(tag==CHAR_TAG_TITRE	)	);
}
//-------------------------------------------
pt_critere critere_creer(void){
	//on utilise calloc pour avoir 1 espace contigu de taille de la struct tout a 0
	//sil plante il va renvoyer NULL par lui meme
	return (pt_critere)calloc( 1 , sizeof(struct critere) );
}
//-------------------------------------------
extern void critere_liberer(void* critere)
{
	//libere le data
	table_string_liberer(((pt_critere)critere)->annee);
	table_string_liberer(((pt_critere)critere)->personne);
	table_string_liberer(((pt_critere)critere)->cote);
	table_string_liberer(((pt_critere)critere)->titre);
	free(((pt_critere)critere)->type_titre);

	//libere le pointeur
	free((pt_critere)critere);
	critere=NULL;
}
//----------------------------------------------
cstr critere_get_type_titre(pt_critere const critere){
	return (
		critere
		?	critere->type_titre
		:	NULL
	);
}
//---------------------------------------------
BOOL critere_set_type_titre(pt_critere const critere, cstr type_titre){
	if(!( critere && type_titre ) )
		return FALSE;

	if(critere->type_titre){
		free(critere->type_titre);
		critere->type_titre=NULL;
	}

	return (
		( ( critere->type_titre=strdup(type_titre) ) == NULL )
		?	FALSE
		:	TRUE
	);
}
//-------------------------------------------
unsigned int critere_set(	pt_critere 	const 	critere			,
							cstr 		s_tuple_critere	,
							t_tag_critere 		tag				)
{	
	h_table_string* selection=NULL;

	//validation des entrees et de la selection
	if(	!s_tuple_critere 	|| !*s_tuple_critere	|| !critere	 		||
		!( selection=critere_select_table_string_handle(critere,tag) )	){
		return 0;
	}

	//on reset la table de string
	if(*selection){
		table_string_liberer(*selection);
	}

	//retourne le BOOLeen du succes de lallocation
	return (((*selection)=table_string_creer(s_tuple_critere,S_CRITERE_TOKEN_C)) != NULL);
}
//-------------------------------------------
unsigned int critere_get_taille(	t_tag_critere 		tag		, 
									pt_critere 	const 	critere	){
	return (unsigned int)table_string_get_taille(critere_get_table_string(critere,tag));
}
//-------------------------------------------
cstr critere_get_element_i(	pt_critere 	const 	critere	, 
							t_tag_critere 		tag		, 
							unsigned int 		i		){
	//on transfert la validation aux deux fonctions secures ci dessous
	return table_string_get_element_n(critere_get_table_string( critere, tag ) , i );
}
//-------------------------------------------
unsigned int critere_foreach( 	pt_critere 	const 	critere		,
								const fp_val_ctx 	f_do		,
								t_tag_critere		tag			, 	
								void 				*f_data		)
{
	//selection du critere
	h_table_string* selection =critere_select_table_string_handle(critere,tag);

	return (
		( selection && *selection )						  /* validation 			  */
		?	table_string_foreach(*selection,f_do,f_data) /* iter et retourne nb iter */
		:	0											  /* aucunes iter retourne 0  */
	);
}
//-------------------------------------------
unsigned int critere_contient( 	pt_critere 	const 		critere	, 
								t_tag_critere 			tag,
								const fp_comp			f_comparer	,
								cstr					f_str2		){
	h_table_string* selection=critere_select_table_string_handle(critere,tag);

	return (
		( selection && *selection )	/* validation 				 */
		?	table_string_contient(	/* iter et retourne nb iter */
				*selection,
				f_comparer,
				f_str2
			)
		:	0						/* aucunes iter retourne 0  */
	);
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*					IMPLEMENTATION FONCTIONS DE SERIALISATION 							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

str critere_serialiser(pt_critere const critere){
	cstr 		sep   =$(sc);
	const char 	map[4]={tag_titre,tag_personne,tag_annee,tag_cote};
	int 		taille=3*strlen(sep)+1;//3separateurs plus 1\0
	const str   ser[4]={0};

	if(!critere)
		return NULL;
	
	for(int i=0;i<4;++i){
		ser[i]=fstr_flatten_table_string_make(
			critere_get_table_string(critere,map[i]),
			$(st)
		);
		taille+=ser[i]?strlen(ser[i]):0;
	}
	if(!(buf)=calloc(taille,sizeof(char))){
		for(int i=0;i<4;++i)
			free(ser[i]);
		return NULL;
	};
	int len=0;
	for(int i=0;i<4;++i){
		strcpy(buf+len,ser[i]?ser[i]:"");
		len+=strlen(ser[i]);

		if(i=4) break;

		strcpy(buf+len,$(sc));
		len+=strlen($(sc));
	}

	return str;
}
pt_critere critere_deserialiser(cstr buf){
	pt_critere 		crit;
	h_table_string 	tc;
	const char		map[4]={tag_titre,tag_personne,tag_annee,tag_cote};

	if(!buf)									goto cleanup0;
	if(!(crit=critere_creer()))					goto cleanup0;
	if(!(tc=table_string_creer(buf,$(sc)))	||
		table_string_get_taille(tc)!=4		)	goto cleanup1;


	for(int i=0;i<4;++i){
		if(!(critere_set(crit,table_string_get_element_n(tc,i),map[i])))
			goto cleanup1;
	}

	table_string_liberer(tc);
	return crit;

	cleanup1:table_string_liberer(tc);
	cleanup0:critere_liberer(crit);

	return NULL;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/




/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*					IMPLEMENTATION FONCTIONS PRIVEES           							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
h_table_string 	critere_get_table_string(pt_critere const critere,t_tag_critere tag){
	return *critere_select_table_string_handle(critere,tag);
}
//-------------------------------------------
h_table_string* critere_select_table_string_handle(
	pt_critere 	const critere,
	t_tag_critere tag
){
	if(!critere||!tag_critere_valide(tag)){
		return NULL;
	}
	switch(tag){
		case tag_titre:		return &(critere->titre);
		case tag_cote:		return &(critere->cote);
		case tag_personne:	return &(critere->personne);
		case tag_annee:		return &(critere->annee);
		default:			return NULL;
	}
}
//-------------------------------------------
t_tag_critere critere_prochain_tag_boucle(t_tag_critere tag)
{
	// Machine d'etat de changement des tag en boucle.
	switch(tag){
		case tag_titre:		return tag_cote;
		case tag_cote:		return tag_personne;
		case tag_personne:	return tag_annee;
		case tag_annee:		return tag_titre;
		default:			return tag_titre;
	}
}
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


