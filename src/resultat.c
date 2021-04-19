/*
	Projet		: ELE216 - Laboratoire 1
	Date		: 09 fevrier 2021
	Par 		: Gabriel Gandubert et Hugo Cusson-Bouthillier
	Logiciel	: Sublime text & Eclipse
	Definition	: Encapsulation des resultats de la recherche et ecriture des resultats
					dans des fichiers texte.

	Contient:
		Structure de subdivision des lignes de chaque base de donnee (titres, noms et 
		cotes) afin de les accumuler et les comparer entres elles pour sortir le
		resultat final.


		FONCTIONS PUBLIQUES:
			
		%% BASE DE DONNEE DES TITRES %%

			% Creation d'une structure pour les titres.
			h_donnee_titre donnee_titre_creer(	const char* const id_titre,
										const char* const titre,
										const char* const annee,
										const char* const genre,
										const char* const ligne
								 )
			% Liberation du contenu de la structure.
			void donnee_titre_liberer(h_donnee_titre donnee)
			% Retourne le numero de reference du titre.
			const char* const donnee_titre_get_id_titre(const h_donnee_titre const donnee)
			% Retourne le titre du film
			const char* const donnee_titre_get_titre(const h_donnee_titre const donnee)
			% Retourne l'annee de sortie du film.
			const char* const donnee_titre_get_annee(const h_donnee_titre const donnee)
			% Retourne la ligne de la base de donne d'ou elle provient.
			const char* const donnee_titre_get_ligne(const h_donnee_titre const donnee)
			% Ecriture dans un fichier texte des resultats des titres.
			void donnee_afficher_titre(		void* contexte, 
									const char* const cle, 
									void* valeur
						  )

		%% BASE DE DONNEE DES NOMS %%
			
			% Creation d'une structure pour les noms.
			h_donnee_nom donnee_nom_creer(const char* const id_nom,
											const char* const nom,
											const char* const id_titres,
											const char* const ligne
										 )
			% Liberation du contenu de la strucutre
			void                donnee_nom_liberer(h_donnee_nom       donnee)
			% Retourne le numero de reference du titre.
			const char* const   donnee_nom_get_id_nom(const h_donnee_nom const donnee)
			Retourne le nom de l'acteur
			const char* const   donnee_nom_get_nom(const h_donnee_nom const donnee)
			% Retourne le numero de reference du titre.
			const char* const   donnee_nom_get_id_titres(const h_donnee_nom const donnee)
			% Retourne la ligne de la base de donne d'ou elle provient.
			const char* const   donnee_nom_get_ligne(const h_donnee_nom const donnee)
			% Ecriture dans un fichier texte des resultats des noms.
			void donnee_afficher_nom(		void* contexte, 
									const char* const cle, 
									void* valeur
						  )
			

		%% BASE DE DONNEE DES COTES %%
			
			% Creation d'une structure pour les cotes.
			h_donnee_cote	  donnee_cote_creer(		const char* const id_titre,
														const char* const cote,
														const char* const ligne
												 )
			% Liberation du contenu de la strucutre
			void                donnee_cote_liberer( h_donnee_cote       donnee)
			% Retourne le numero de reference du titre.
			const char* const donnee_cote_get_id_titre(const h_donnee_cote const donnee)
			% Retourne la cote du film
			const char* const donnee_cote_get_cote(	const h_donnee_cote const donnee)
			% Retourne la ligne de la base de donne d'ou elle provient.
			const char* const donnee_cote_get_ligne(const h_donnee_cote const donnee)
			% Ecriture dans un fichier texte des resultats des cotes.
			void donnee_afficher_cote(		void* contexte, 
									const char* const cle, 
									void* valeur
						  )
			

*/

// Header file.
#include "resultat.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							CONSTANTES D'IMPLEMENTATION     							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Constante de limite de taille des chaines de caracteres (taille max de la ligne).
#define TAILLE_MAX_CHAINE 1024

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							TYPES ET STRUCTURES OPAQUES     							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*
 * @brief	: Structure des donnees du fichier des titres
 * @note	: HLR20 structure des resultats bases sur les titres
 */
struct donnee_titre
{
	char* id_titre;	// Numero de reference des titres.
	char* titre;	// Nom du titre.
	char* annee;	// Annee de sortie.
	char* ligne;	// Ligne complete de la base de donnee.
};

/*
 * @brief	: Structure des donnees du fichier des noms
 * @note	: HLR19 structure des resultats bases sur les noms
 */
struct donnee_nom
{
	char* 			id_nom;		// Numero de reference du nom.
	char* 			nom;		// Nom de l'acteur.
	char* 			id_titres;	// Numero de reference des titres.
	char* 			ligne;		// Ligne complete de la base de donnee.
};

/*
 * @brief	: Structure des donnees du fichier des cotes
 * @note	: HLR21 structure des resultats bases sur les cotes
 */
struct donnee_cote
{
	char* id_titre;	// Numero de reference des titres.
	char* cote;		// Cote recu par le film.
	char* ligne;	// Ligne complete de la base de donnee.
};

/*
 * @brief	: Structure des donnees combinees
 * @note	: HLR22 structure des resultats bases sur la compilation des donnees
 */
struct donnee_resultat
{
	char* 			id_titre;	// Numero de reference des titres.
	char* 			titre;		// Nom du titre.
	char* 			annee;		// Annee de sortie.
	char* 			nom;		// Nom de l'acteur
	char* 			cote;		// Cote du film
	char* 			ligne_titre;// Ligne complete de la base de donnee.
};
/**
 * @brief   : structure de lignes combinées
 * @note    : Serveur-HLR06 - définition d'une de lignes combinées
 */
struct resultat_ligne{
	h_list titre;
	h_list cote;
	h_list nom;
};
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*							IMPLLEMENTATION FONCTIONS PUBLIQUES							*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Lecture de la taille du type t_donnee_titre.
size_t donnee_titre_sizeof(){
	return sizeof(t_donnee_titre);
}

// Creation d'une structure de donnee du fichier des titres.
h_donnee_titre donnee_titre_creer(	const char* const id_titre,
										const char* const titre,
										const char* const annee,
										const char* const ligne
								 )
{
	// Verification des initialisateurs non nuls ou trop grandes.
	if(id_titre == NULL || titre == NULL || annee == NULL || ligne == NULL)
	{
		return NULL;
	}

	// Verification de la longeur de la chaine de caractere.
	if(strlen(id_titre) > TAILLE_MAX_CHAINE ||
		strlen(titre) 	> TAILLE_MAX_CHAINE ||
		strlen(annee) 	> TAILLE_MAX_CHAINE ||
		strlen(ligne) 	> TAILLE_MAX_CHAINE)
	{
		return NULL;
	}

	// Allocation dynamique du nouveau resultat.
	h_donnee_titre donnee = (h_donnee_titre) malloc(sizeof(t_donnee_titre));
	
	// Verification de l'allocation correct.
	if(donnee == NULL)
	{
		return NULL;
	}

	// Initialisation des valeurs.
	donnee->id_titre 	= strdup(id_titre);
	donnee->titre 		= strdup(titre);
	donnee->annee		= strdup(annee);
	donnee->ligne		= strdup(ligne);

	// Retourne la structure.
	return donnee;
}

// Fonction de liberation d'une strucure de donne des titres.
extern void donnee_titre_liberer(void* donnee)
{
	h_donnee_titre p = (h_donnee_titre) donnee;
	if(p != NULL)
	{
		free(p->id_titre);
		free(p->titre);
		free(p->annee);
		free(p->ligne);
		free(p);
		p = NULL;
	}
}

// ENSEMBLE DES FONCTIONS "GET" POUR LES DONNEES DU FICHIER TITRE.

// Extrait de la structure l'identifiant du titre.
const char* const   donnee_titre_get_id_titre(	const h_donnee_titre const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->id_titre);
	}
	return NULL;
}

// Extrait de la structure le titre.
const char* const   donnee_titre_get_titre(		const h_donnee_titre const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->titre);
	}
	return NULL;
}

// Extrait de la structure l'annee.
const char* const   donnee_titre_get_annee(		const h_donnee_titre const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->annee);
	}
	return NULL;
}

// Extrait de la structure la ligne.
const char* const	donnee_titre_get_ligne(		const h_donnee_titre const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->ligne);
	}
	return NULL;
}

// Ecriture dans un fichier texte des resltats recu par le programme.
extern void donnee_afficher_titre(		void* contexte, 
									const char* const cle, 
									void* valeur
						  )
{
	// Assignation des types de valeurs de la fonction
	FILE* fichier = (FILE*) contexte;
	h_donnee_titre donnee = (h_donnee_titre) valeur;

	// Ecriture dans le fichier.
	fprintf(contexte, "%s\n", donnee_titre_get_ligne(donnee));
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Lecture de la taille du type t_donnee_nom.
size_t donnee_nom_sizeof(){
	return sizeof(t_donnee_nom);
}

// Creation d'une structure de donnee du fichier des noms.
h_donnee_nom donnee_nom_creer(	const char* const id_nom,
											const char* const nom,
											const char* const id_titres,
											const char* const ligne
									)
{
	// Verification des initialisateurs non nuls.
	if(id_nom == NULL || nom == NULL || id_titres == NULL || ligne == NULL)
	{
		return NULL;
	}

	// Verification de la longeur de la chaine de caractere.
	if(strlen(id_nom)		> TAILLE_MAX_CHAINE ||
		strlen(nom)			> TAILLE_MAX_CHAINE ||
		strlen(id_titres)	> TAILLE_MAX_CHAINE ||
		strlen(ligne)		> TAILLE_MAX_CHAINE)
	{
		return NULL;
	}
	// Allocation dynamique du nouveau resultat.
	h_donnee_nom donnee = (h_donnee_nom) malloc(sizeof(t_donnee_nom));
	
	// Verification de l'allocation correct.
	if(donnee == NULL)
	{
		return NULL;
	}

	// Initialisation des valeurs.
	donnee->id_nom	 	= strdup(id_nom);
	donnee->nom 		= strdup(nom);
	donnee->id_titres 	= strdup(id_titres);
	donnee->ligne		= strdup(ligne);

	// Retourne la structure.
	return donnee;
}

// Fonction de liberation d'une strucure de donne des noms.
extern void donnee_nom_liberer(void* donnee)
{
	h_donnee_nom p = (h_donnee_nom) donnee;

	if(p != NULL)
	{
		free(p->id_nom);
		free(p->nom);
		free(p->id_titres);
		free(p->ligne);
		free(p);
		p = NULL;
	}
}

// ENSEMBLE DES FONCTIONS "GET" POUR LES DONNEES DU FICHIER NOM.

// Extrait de la structure l'identifiant du nom.
const char* const   donnee_nom_get_id_nom(   	const h_donnee_nom const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->id_nom);
	}
	return NULL;
}

// Extrait de la structure le nom.
const char* const   donnee_nom_get_nom(      	const h_donnee_nom const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->nom);
	}
	return NULL;
}

// Extrait de la structure l'identifiant du titre.
const char* const   donnee_nom_get_id_titres(	const h_donnee_nom const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->id_titres);
	}
	return NULL;
}

// Extrait de la structure la ligne.
const char* const   donnee_nom_get_ligne(		const h_donnee_nom const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->ligne);
	}
	return NULL;
}

// Ecriture dans un fichier texte des resltats recu par le programme.
extern void donnee_afficher_nom(		void* contexte, 
									const char* const cle, 
									void* valeur
						)
{
	// Assignation des types de valeurs de la fonction
	FILE* fichier = (FILE*) contexte;
	h_donnee_nom donnee = (h_donnee_nom) valeur;

	// Ecriture dans le fichier.
	fprintf(contexte, "%s\n", donnee_nom_get_ligne(donnee));
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Lecture de la taille du type t_donnee_cote.
size_t donnee_cote_sizeof(){
	return sizeof(t_donnee_cote);
}

// Creation d'une structure de donnee du fichier des cotes.
h_donnee_cote donnee_cote_creer(	const char* const id_titre,
										const char* const cote,
										const char* const ligne
							   )
{
	// Verification des initialisateurs non nuls.
	if(id_titre == NULL || cote == NULL || ligne == NULL)
	{
		return NULL;
	}

	// Verification de la longeur de la chaine de caractere.
	if(strlen(id_titre)		> TAILLE_MAX_CHAINE ||
		strlen(cote)		> TAILLE_MAX_CHAINE ||
		strlen(ligne)		> TAILLE_MAX_CHAINE)
	{
		return NULL;
	}
	// Allocation dynamique du nouveau resultat.
	h_donnee_cote donnee = (h_donnee_cote) malloc(sizeof(t_donnee_cote));
		
	// Verification de l'allocation correct.
	if(donnee == NULL)
	{
		return NULL;
	}

	// Initialisation des valeurs.
	donnee->id_titre	= strdup(id_titre);
	donnee->cote		= strdup(cote);
	donnee->ligne		= strdup(ligne);

	// Retourne la structure.
	return donnee;
}

// Fonction de liberation d'une strucure de donne des cotes.
extern void donnee_cote_liberer(void* donnee)
{
	h_donnee_cote p = (h_donnee_cote) donnee;

	if(p != NULL)
	{
		free(p->id_titre);
		free(p->cote);
		free(p->ligne);
		free(p);
		p = NULL;
	}
}

// ENSEMBLE DES FONCTIONS "GET" POUR LES DONNEES DU FICHIER COTE.

// Extrait de la structure l'identifiant du titre.
const char* const   donnee_cote_get_id_titre(	const h_donnee_cote const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->id_titre);
	}
	return NULL;
}

// Extrait de la structure la cote.
const char* const   donnee_cote_get_cote(	const h_donnee_cote const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->cote);
	}
	return NULL;
}

// Extrait de la structure la ligne.
const char* const   donnee_cote_get_ligne(		const h_donnee_cote const donnee)
{
	if(donnee != NULL)
	{
		return (donnee->ligne);
	}
	return NULL;
}

// Ecriture dans un fichier texte des resltats recu par le programme.
extern void donnee_afficher_cote(		void* contexte, 
									const char* const cle, 
									void* valeur
						  )
{
	// Assignation des types de valeurs de la fonction
	FILE* fichier = (FILE*) contexte;
	h_donnee_cote donnee = (h_donnee_cote) valeur;

	// Ecriture dans le fichier.
	fprintf(contexte, "%s\n", donnee_cote_get_ligne(donnee));
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Lecture de la taille du type t_donnee_resultat.
size_t donnee_resultat_sizeof(){
	return sizeof(t_donnee_resultat);
}

// Creation d'une structure de donnee du fichier des noms.
h_donnee_resultat donnee_resultat_creer(void)
{
	// Allocation dynamique du nouveau resultat.
	h_donnee_resultat donnee = (h_donnee_resultat) malloc(sizeof(t_donnee_resultat));
	
	// Verification de l'allocation correct.
	if(donnee == NULL)
	{
		return NULL;
	}

	// Initialisation des valeurs.
	donnee->id_titre	= NULL;
	donnee->titre 		= NULL;
	donnee->nom 		= NULL;
	donnee->annee 		= NULL;
	donnee->cote 		= NULL;
	donnee->ligne_titre	= NULL;

	// Retourne la structure.
	return donnee;
}

// Libere la structure de donnee.
extern void donnee_resultat_liberer(void* donnee)
{
	h_donnee_resultat p = (h_donnee_resultat) donnee;

	// Liberation de chaque element si l'allocation a eu lieu.
	if(p != NULL)
	{
		if(p->id_titre != NULL)
		{
			free(p->id_titre);
		}
		if(p->titre != NULL)
		{
			free(p->titre);
		}
		if(p->annee != NULL)
		{
			free(p->annee);
		}
		if(p->nom != NULL)
		{
			free(p->nom);
		}
		if(p->cote != NULL)
		{
			free(p->cote);
		}
		if(p->ligne_titre != NULL)
		{
			free(p->ligne_titre);
		}
		free(p);
		p = NULL;
	}
}

// ENSEMBLE DES FONCTIONS "GET" POUR LA COMBINAISON DES FICHIERS.

// Extrait de la structure l'identifiant du titre.
const char* const   donnee_resultat_get_id_titre(	const h_donnee_resultat const donnee)
{
	if(donnee != NULL && donnee->id_titre != NULL)
	{
		return (donnee->id_titre);
	}
	return NULL;
}

// Extrait de la structure le titre.
const char* const   donnee_resultat_get_titre(		const h_donnee_resultat const donnee)
{
	if(donnee != NULL && donnee->titre != NULL)
	{
		return (donnee->titre);
	}
	return NULL;
}

// Extrait de la structure l'annee.
const char* const   donnee_resultat_get_annee(		const h_donnee_resultat const donnee)
{
	if(donnee != NULL && donnee->annee != NULL)
	{
		return (donnee->annee);
	}
	return NULL;
}

// Extrait de la structure le nom.
const char* const   donnee_resultat_get_nom(		const h_donnee_resultat const donnee)
{
	if(donnee != NULL && donnee->nom != NULL)
	{
		return (donnee->nom);
	}
	return NULL;
}

// Extrait de la structure la cote.
const char* const   donnee_resultat_get_cote(		const h_donnee_resultat const donnee)
{
	if(donnee != NULL && donnee->cote != NULL)
	{
		return (donnee->cote);
	}
	return NULL;
}

// Extrait de la structure la ligne de titre.
const char* const   donnee_resultat_get_ligne_titre(const h_donnee_resultat const donnee)
{
	if(donnee != NULL && donnee->ligne_titre != NULL)
	{
		return donnee->ligne_titre;
	}
	return NULL;
}

// Assignation de la valeur de l'identifiant du titre dans la structure.
int donnee_resultat_set_id_titre(h_donnee_resultat donnee, const char* const id_titre)
{
	if( id_titre && strlen(id_titre) <= TAILLE_MAX_CHAINE && donnee ){
		if(donnee->id_titre){
			free(donnee->id_titre);
		}
		
		donnee->id_titre = strdup(id_titre);

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

// Assignation de la valeur du titre dans la structure.
int donnee_resultat_set_titre(h_donnee_resultat donnee, const char* const titre)
{
	if( titre && strlen(titre) <= TAILLE_MAX_CHAINE && donnee ){
		if(donnee->titre){
			free(donnee->titre);
		}
		donnee->titre = strdup(titre);

		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

// Assignation de la valeur de l'annee dans la structure.
int donnee_resultat_set_annee(h_donnee_resultat donnee, const char* const annee)
{
	if( annee && strlen(annee) <= TAILLE_MAX_CHAINE && donnee ){
		if(donnee->annee){
			free(donnee->annee);
		}
		donnee->annee = strdup(annee);
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

// Assignation de la valeur du nom dans la structure.
int donnee_resultat_set_nom(h_donnee_resultat donnee, const char* const nom)
{
	if( nom && strlen(nom) <= TAILLE_MAX_CHAINE && donnee ){
		if(donnee->nom){
			free(donnee->nom);
		}
		donnee->nom = strdup(nom);
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

// Assignation de la valeur de la cote dans la structure.
int donnee_resultat_set_cote(h_donnee_resultat donnee, const char* const cote)
{
	if( cote && strlen(cote) <= TAILLE_MAX_CHAINE && donnee ){
		if(donnee->cote){
			free(donnee->cote);
		}
		donnee->cote = strdup(cote);
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

// Assignation de la valeur de la ligne du titre dans la structure.
int donnee_resultat_set_ligne_titre(
	h_donnee_resultat donnee,
	const char* const ligne_titre
){
	if( ligne_titre && strlen(ligne_titre) <= TAILLE_MAX_CHAINE && donnee ){
		if(donnee->ligne_titre){
			free(donnee->ligne_titre);
		}
		donnee->ligne_titre = strdup(ligne_titre);
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

// Ecriture dans un fichier texte des resltats recu par le programme.
extern void donnee_afficher_resultat(void* contexte, const char* const cle, void* valeur)
{
	// Assignation des types de valeurs de la fonction
	FILE* fichier = (FILE*) contexte;
	h_donnee_resultat donnee = (h_donnee_resultat) valeur;

	// Ecriture dans le fichier.
	fprintf(contexte, "%s\n", donnee_resultat_get_ligne_titre(donnee));
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Lecture de la taille du type t_resultat_ligne.
size_t resultat_ligne_sizeof(){
	return sizeof(t_resultat_ligne);
}
// Creation d'une structure de donnee du fichier des noms.
h_resultat_ligne resultat_ligne_creer(void)
{
	// Allocation dynamique du nouveau resultat.
	h_resultat_ligne donnee = (h_resultat_ligne) malloc(sizeof(t_resultat_ligne));
	
	// Verification de l'allocation correct.
	if(donnee == NULL)
	{
		return NULL;
	}

	// Initialisation des valeurs.
	donnee->titre=list_new(free,num_ligne_leq);
	donnee->nom=list_new(free,num_ligne_leq);
	donnee->cote=list_new(free,num_ligne_leq);

	// Retourne la structure.
	return donnee;
}

// Libere la structure de donnee.
extern void        resultat_ligne_liberer( void* donnee)
{
	void** p=donnee;
	if(!donnee)			
		return;
	for(int i=0;i<3;++i) 
		list_free(p[i]);
}
const h_list const resultat_ligne_get_liste_titre( 	const h_resultat_ligne const donnee)
{
	return ( donnee ? donnee->titre : NULL );
}
const h_list const resultat_ligne_get_liste_nom(	const h_resultat_ligne const donnee)
{
	return ( donnee ? donnee->nom : NULL );
}
const h_list const resultat_ligne_get_liste_cote(	const h_resultat_ligne const donnee)
{
	return ( donnee ? donnee->cote : NULL );
}

// Ecriture dans un fichier texte des resltats recu par le programme.
extern void list_resultat_ligne_afficher(void* valeur,void*contexte)
{
	// Ecriture dans le fichier.
	fprintf((FILE*)contexte, "%s", (char*)valeur );
}
extern unsigned num_ligne_leq(void* num_ligne_gauche,void* num_ligne_droite){
	return (atol(num_ligne_gauche+2)<=atol(num_ligne_droite+2)?TRUE:FALSE);
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/