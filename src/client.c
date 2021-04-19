/*
	Projet		: ELE216 - Laboratoire 1
	Date		: 15 fevrier 2021
	Par 		: Gabriel Gandubert et Hugo Cusson-Bouthillier
	Definition	: Encapsulation du type d'operation et des criteres recu par le 
					programme principal.

	Contient:
		Structure des operations qui permet de combiner les types de criteres (commandes)
		et le type d'operation.
		La structure est defini ici, mais le "typedef" est dans le module client.h.

		FONCTIONS PUBLIQUES :
			operation_creer				:	Creation d'une structure des operations.
			operation_liberer			:	Liberation d'une strcuture des operations.
			operation_get_critere_i		:	Lecture d'un critere specifique de la 
											structure interne.
			operation_get_critere		:	Lecture de la structure des criteres.
			operation_get_taille_crit	:	Lecture du nombre d'element pour un critere
											specifique.
			operation_get_t				:	Lecture de l'element T de la structure des
											operations.
			operation_get_type_titre	:	Lecture de l'element type_tire de la structure
											des operations.
			operation_set_t				:	Modificateur de l'element T de la structure
											des operations.
			operation_set_critere		:	Modificateur de la structure des criteres
											de la structure des operations.
			operation_set_type_titre	:	Modificateur de l'element type_titre de la
											structures des operations.
			serialiser_operation		:	Transformation de la structure des operations
											en chaine de caractere pour la communication.
			deserialiser_operation		:	Transformation de la chaine de caractere
											serialisee en structure des operations.
*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Header file
#include "client.h"



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							TYPES ET STRUCTURES OPAQUES     							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief	: 	Structure de donnees permettant de contenir l'operation et les criteres
 * 			  	recu dans le programme principal.
 * @note	:	HLR04 Structure des operations contenant la strucutre des criteres,
 				l'operation choisi et le  type de titre pour l'ajout.
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/




/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							IMPLLEMENTATION FONCTIONS PUBLIQUES							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Creation d'une structure pour l'operation exigee. Par defaut: mode recherche.
pt_operation operation_creer(void)
{
	// on utilise calloc pour avoir 1 espace contigu de taille de la struct tout a 0
	// s'il plante il va renvoyer NULL par lui meme
	return (pt_operation) calloc(1, sizeof(struct operation));
}

//----------------------------------------------------------------------------------------

// Liberation de la structure de l'operation exigee ainsi que le(s) critere(s) contenu(s).
extern void operation_liberer(void* oper)
{	
	pt_operation op=oper;
	// Si la structure est valide, liberer.
	if(op != NULL)
	{
		// Si les criteres sont valides, liberer.
		if(op->critere != NULL)
		{
			critere_liberer(op->critere);
		}

		free(op);
		op = NULL;
	}
}

//----------------------------------------------------------------------------------------

// Retourne un element de la structure des criteres.
const char* operation_get_critere_i(pt_operation oper, t_tag_critere tag, unsigned int i)
{
	if(oper != NULL)
	{
		return critere_get_element_i(oper->critere, tag, i);
	}
	return NULL;
}

//----------------------------------------------------------------------------------------

/* 
 * Retourne le pointeur des criteres. Si l'une des deux structure n'est pas initialisee,
 * La fonction retourne un pointeur NULL.
 */
const pt_critere operation_get_critere(pt_operation oper)
{
	/* 
	 * Retourner le critere si la structure d'operation existe. 
	 * Il est possible que la structure des criteres soit nul.
	 */
	if(oper != NULL)
	{
		return oper->critere;
	}
	return NULL;
}

//----------------------------------------------------------------------------------------

// Retourne la taille du premier tableau du critere selectionnee.
unsigned int operation_get_taille_crit(pt_operation const oper, t_tag_critere tag)
{
	if(oper != NULL && tag)
	{
		return critere_get_taille(tag, oper->critere);
	}

	return 0;
}

//----------------------------------------------------------------------------------------

/* 
 * Retourne le type d'operation voulu dans la structure. 
 * si la structure n'existe pas, la fonction retourne 0.
 */
const type_alias operation_get_t(pt_operation oper)
{
	if(oper != NULL)
	{
		return (const type_alias) oper->T;
	}
	return (const type_alias) EMPTY_ARGUMENT;
}

//----------------------------------------------------------------------------------------

// Retroune le type de titre de la structure.
const char* operation_get_type_titre(pt_operation oper)
{
	if(oper != NULL)
	{
		return (const char*) critere_get_type_titre(oper->critere);
	}
	return NULL;
}

//----------------------------------------------------------------------------------------

// Changement du type d'operation de la structure des operations.
unsigned int operation_set_t(pt_operation oper, type_alias T)
{
	// Verification de l'existance de la structure d'operation.
	if(oper != NULL)
	{
		// Verification du type de recherche attendu.
		if(T == RECHERCHE || T == RETRAIT|| T == AJOUT)
		{
			oper->T = T;
			return OPERATION_SUCCESS;
		}

		// Argument null ou incorrect seront traitees de la meme facon.
		else
		{
			return EMPTY_ARGUMENT;
		}
	}

	return EMPTY_POINTER;
}

//----------------------------------------------------------------------------------------

// Changement des criteres de la structure des operations.
unsigned int operation_set_critere(pt_operation oper, pt_critere critere)
{
	// Verification de l'existance de la structure d'operation.
	if(oper != NULL)
	{
		if(critere == NULL)
		{
			return EMPTY_ARGUMENT;
		}

		// Verification si la structure de critere est deja present.
		if(oper->critere == NULL)
		{
			oper->critere = critere;
			return OPERATION_SUCCESS;
		}
		
		// Si la structure existe, modifier le contenu.
		else
		{
			critere_liberer(oper->critere);
			oper->critere = critere;
			return OPERATION_SUCCESS;
		}
	}

	return EMPTY_POINTER;
}

//----------------------------------------------------------------------------------------

// Changement du type de titre de la structure des operations.
unsigned int operation_set_type_titre(pt_operation oper, char* type_titre)
{
	// Verification de l'existance de la structure d'operation.
	if(oper == NULL)
		return EMPTY_POINTER;

	// Verification de l'existance du type de titre.
	if(type_titre == NULL)
	{
		return EMPTY_ARGUMENT;
	}

	// Verification si la structure de critere est deja present.
	if( oper->critere == NULL ){
		return EMPTY_POINTER;
	}

	return (
		critere_set_type_titre(oper->critere,type_titre)==TRUE
		?	OPERATION_SUCCESS
		:	EMPTY_POINTER
	);
}

//----------------------------------------------------------------------------------------

// Serialisation de la structure des operations.
str serialiser_operation(pt_operation oper)
{
	str  buf     =NULL;		// chaine de caractere tampon pour la sortie.
	str  buf2	 =NULL;
    BOOL success =TRUE;		// Variable BOOLeen pour indiquation de reussite.
    char type_operation[2] = "\0"; // Tableau pour ajouter le type d'operation.

	if(!oper)
		return NULL;
	
	if(!(buf=critere_serialiser(oper->critere)))
		return NULL;
	if(!(buf2=calloc(strlen(buf)+2,sizeof(char)))){
		free(buf);
		return NULL;
	}
	strcpy(buf2+1,buf);
	*buf2=oper->T;
	free(buf);
	
	return buf2;
}

//----------------------------------------------------------------------------------------

// Deserialisation d'une chaine de caracteres en une structure des operations.
pt_operation deserialiser_operation(cstr serie){	

	pt_operation 	oper 			= operation_creer();	// Structure des operations a retourner
	char 			type_titre[2]	= {0,0};
	// Validation des parametres et des allocations dynamiques.
	if(!serie || !oper){
		return NULL;
	}

	*type_titre=(oper->T=*serie);
	
	oper->critere=critere_deserialiser(serie+1);
	if(!oper->critere)
		free(oper);
	
	critere_set_type_titre(oper->critere,type_titre);

	return oper;
}

//----------------------------------------------------------------------------------------
