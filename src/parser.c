/*
    Projet      : ELE216 - Laboratoire 1
    Date        : 21 janvier 2021
    Par         : Gabriel Gandubert et Hugo Cusson-Bouthillier
    Logiciel    : Sublime text & Eclipse
    Definition  : Encapsulation des criteres de recherche pour la base de donnee de films

    Contient:
        Structure des criteres qui permet de separer les types de criteres (commandes)
        La structure est defini ici, mais le "typedef" est dans le module critere.h.

        FONCTIONS PUBLIQUES:
            % Division et comparaison d'une ligne du fichier des titres.
            h_donnee_titre parse_ligne_titre_et_critere(const char* const ligne, 
                                            pt_critere const  critere);
            % Division et comparaison d'une ligne du fichier des noms.
            h_donnee_nom   parse_ligne_nom_et_critere(  const char* const ligne, 
                                            pt_critere const  critere);
            % Division et comparaison d'une ligne du fichier des cotes.
            h_donnee_cote  parse_ligne_cote_et_critere( const char* const ligne, 
                                            pt_critere const  critere);
        FONCTIONS PRIVEEs :
            % Compare la chaine 1 avec la chaine 2 afin de determiner si la chaine 1 est
            comprise dans la chaine 2.
            unsigned int str1_dans_str2(const char* str1, const char* str2);
            % Compare la chaine 1 avec la chaine 2 afin de determiner si la chaine 2 est
            unsigned int str2_dans_str1(const char* str1, const char* str2);
            % Compare l'annee et l'interval afin de determiner si l'annee est comprise
            dans l'interval donne.
            unsigned int annee_dans_interval(const char* interval, const char* annee);
            % Compare la cote et l'interval afin de determiner si la cote est comprise
            dans l'interval donne.
            unsigned int cote_dans_interval(const char* interval, const char* cote);
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Header file
#include "modules/parser.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                          CONSTANTES D'IMPLEMENTATION                                 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Separateur des elements de la base de donne de type TSV (tab separated values).
#define TOKEN_TSV '	'

// Constantes specifiques a la base de donnees des titres de films.
#define NOMBRE_COLONNE_TITRE        9
#define TITRE_INDEX_COLONNE_TITRE   2
#define TITRE_INDEX_COLONNE_ID      0
#define TITRE_INDEX_COLONNE_ANNEE   5

// Constantes specifiques a la base de donnees des noms d'acteurs.
#define NOMBRE_COLONNE_NOM          6
#define NOM_INDEX_COLONNE_ID        0
#define NOM_INDEX_COLONNE_NOM       1
#define NOM_INDEX_COLONNE_TITRES    5

// Constantes specifiques a la base de donnees des cotes des films.
#define NOMBRE_COLONNE_COTE          3
#define COTE_INDEX_COLONNE_ID_TITRE  0
#define COTE_INDEX_COLONNE_COTE      1

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                          IMPLLEMENTATION FONCTIONS PUBLIQUES                         */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*
 * @note HLR17: Parser de la base de donnee des titres de films.
 */
h_donnee_titre parse_ligne_titre_et_critere( 
    const char* const ligne     , 
    pt_critere const  critere   
){  

    // Variables de separation de la ligne en donnees
    h_table_string table_ligne;
    h_donnee_titre donnees_ligne;

    // Verification des tableau pour des valeurs non nul.
    if( !ligne || !critere)
        return NULL;

    // Separation de la ligne en colonne et validation de l'operation.
    if( !( table_ligne = table_string_creer(ligne,"\t") )        ||  
        table_string_get_taille(table_ligne) != NOMBRE_COLONNE_TITRE  ){
        return NULL;
    }
    
    // Extraction des donnees pertinentes de la ligne afin de comparer.
    if(!( donnees_ligne=donnee_titre_creer( 
            table_string_get_element_n(table_ligne,TITRE_INDEX_COLONNE_ID),
            table_string_get_element_n(table_ligne,TITRE_INDEX_COLONNE_TITRE),
            table_string_get_element_n(table_ligne,TITRE_INDEX_COLONNE_ANNEE),
            ligne
    ))){
        table_string_liberer(table_ligne);
        free(table_ligne);
        return NULL;
    }

    /*
     * @note HLR29: Appel des fonctions de comparaison dans parser.
     */
    // Comparaison des donnees de la ligne avec les criteres.
    if(!comparer_donnee_titre_critere(donnees_ligne,critere)){
        table_string_liberer(table_ligne);
        donnee_titre_liberer(donnees_ligne);
        return NULL;
    }

    // Liberation du tableau de la ligne separee.
    table_string_liberer(table_ligne);

    // La comparaison a trouver une ligne respectant les criteres.
    return donnees_ligne;
}

//----------------------------------------------------------------------------------------

/*
 * @note HLR16: Parser de la base de donnee des noms d'acteurs.
 */
h_donnee_nom   parse_ligne_nom_et_critere(  
    const char* const       ligne     , 
    pt_critere const  critere   
){

    // Variables de separation de la ligne en donnees
    h_donnee_nom  donnees_ligne;
    h_table_string table_ligne;

    // Verification des tableau pour des valeurs non nul.
    if( !ligne || !critere)
        return NULL;

    // Separation de la ligne en colonne
    table_ligne=table_string_creer(ligne,"\t");
    
    // Validation de la ligne separee et du nombre de colonnes trouvee.
    if(     !table_ligne 
        ||  table_string_get_taille(table_ligne)!=NOMBRE_COLONNE_NOM  ){
        return NULL;
    }
    
    // Extraction des donnees de la ligne necessaire uniquement.
    donnees_ligne=donnee_nom_creer( 
        table_string_get_element_n(table_ligne,NOM_INDEX_COLONNE_ID),
        table_string_get_element_n(table_ligne,NOM_INDEX_COLONNE_NOM),
        table_string_get_element_n(table_ligne,NOM_INDEX_COLONNE_TITRES),
        ligne
    );

    // Si l'extraction a echouee, echec.
    if(!donnees_ligne){
        table_string_liberer(table_ligne);
        free(table_ligne);
        return NULL;
    }

    /*
     * @note HLR29: Appel des fonctions de comparaison dans parser.
     */
    // Comparaison des criteres avec les donnees de la ligne.
    if(!comparer_donnee_nom_critere(donnees_ligne,critere)){
        table_string_liberer(table_ligne);
        donnee_nom_liberer(donnees_ligne);
        return NULL;
    }

    // Envoyer les donnees valides.
    return donnees_ligne;
}

//----------------------------------------------------------------------------------------

/*
 * @note HLR18: Parser de la base de donnee des cotes de films.
 */
h_donnee_cote  parse_ligne_cote_et_critere( 
    const char* const       ligne     , 
    pt_critere const  critere   
){

    // Variables de separation de la ligne en donnees
    h_donnee_cote  donnees_ligne=NULL;
    h_table_string table_ligne=NULL;

    // Verification des tableau pour des valeurs non nul.
    if( !ligne || !critere)
        return NULL;

    // Separation de la ligne en colonne
    table_ligne=table_string_creer(ligne,"\t");
    
    // Validation de la ligne separee et du nombre de colonnes trouvee.
    if(     !table_ligne 
        ||  table_string_get_taille(table_ligne)!=NOMBRE_COLONNE_COTE  ){
        return NULL;
    }
    
    // Extraction des donnees de la ligne necessaire uniquement.
    donnees_ligne=donnee_cote_creer( 
        table_string_get_element_n(table_ligne,COTE_INDEX_COLONNE_ID_TITRE),
        table_string_get_element_n(table_ligne,COTE_INDEX_COLONNE_COTE),
        ligne
    );

    // Si l'extraction a echouee, echec.
    if(!donnees_ligne){
        table_string_liberer(table_ligne);
        free(table_ligne);
        return NULL;
    }

    /*
     * @note HLR29: Appel des fonctions de comparaison dans parser.
     */
    // Comparaison des criteres avec les donnees de la ligne.
    if(!comparer_donnee_cote_critere(donnees_ligne,critere)){
        table_string_liberer(table_ligne);
        donnee_cote_liberer(donnees_ligne);
        return NULL;
    }

    // Envoyer les donnees valides.
    return donnees_ligne;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/




