#ifndef __DBSEARCH__H__
#define __DBSEARCH__H__

#include "base.h"
#include "csv_parser.h"
#include "csv_handlers.h"

// Constantes specifiques a la base de donnees des titres de films.
#define NOMBRE_COLONNE_TITRE        9
#define TITRE_INDEX_COLONNE_ID      0
#define TITRE_INDEX_COLONNE_TITRE   2
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

void search(void){
    
}

#endif  //!__DBSEARCH__H__