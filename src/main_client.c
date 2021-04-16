/*
 *  main.c
 *	
 *	Project 	: ELE216 - Laboratoire 2
 *  Created on	: 11 fevrier 2021
 *  Auteurs		: Hugo Cusson-Bouthillier & Gabriel Gandubert
 *
 *	Ce programme recois une liste de parametres contenant des commandes specifiques
 *	afin d'indiquer le type d'operation a faire et du type de critere de recherche,
 *	d'ajout, ou de suppression pour une base de donnee de films.
 *
 *	Ce programme ne contient que la phase 1 du projet.
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// Appel des librairies necessaire pour ce programme.
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

// Librairires standard Pour la communication (Phase 2)
#include <pthread.h>
#include <errno.h>
#include <flock.h>
#include <modules/tetris.h>

// Liste des modules necessaire pour ce programme.
#include "util/table_string.h"
#include "modules/critere.h"
#include "util/hashmap.h"
#include "modules/client.h"
#include "debug_include.h"

// Pour la communication (Phase 2)
#include "modules/transmission.h"
#include "util/linklist.h"



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                           Fonctions locales                                      */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief  implementation dune fonction pour etre employee dans critere_foreach
 * 			La fonction imprime a la console a string quon lui passe en argument
 * @param  critere	:la string a imprimer
 * @param  not_used	:pas utilise. necessaire pour le format de foncteur de critere_foreach
 * @retval None
 */
extern unsigned int imprimer_critere_foreach(const char* critere,void* not_used){
	if(critere)printf("    %s\n",critere);
	
	return EXIT_SUCCESS;
};

// Deplacer!!!
typedef struct comm_serveur_struct t_comm_serveur, h_comm_serveur;

	struct comm_serveur_struct{
		h_tx_msg liste_msg;
		h_public_file_system pub_file;
		void* ret_val
	}

/**
 * @brief  Fonction permettant dimprimer le contenu dun critere a la console
 * @note   HLR09-01	: Fonction temporaire pour afficher le contenu de la structure.
 * @param  critere	: le critere a imprimer
 * @retval None
 */
void imprimer_critere(pt_critere const critere){

	t_fp_foreach f=&imprimer_critere_foreach;
	printf("Titre : \n");
	critere_foreach(critere,f,tag_titre,NULL);
	printf("Personne : \n");
	critere_foreach(critere,f,tag_personne,NULL);
	printf("Annee : \n");
	critere_foreach(critere,f,tag_annee,NULL);
	printf("Cote : \n");
	critere_foreach(critere,f,tag_cote,NULL);
};

//------------------------------------------------------------------------------------

/**
 * @brief  Appelee par le thread de gestion de la communication avec le serveur.
 * @param  arg	: les arguments provenant du pthread_create();.
 * @retval None
 */
void* comm_serveur(void* arg);

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                           Fonction main                                          */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
int main(int argc, char **argv){

	// Thread pour la communication avec le serveur.
	pthread_t thread_serveur;

	// Variables des structures des commandes/arguments de l'utilisateur.
	pt_critere		critere		= NULL;
	pt_operation	operation	= NULL;

	// Variable de reception d'un argument de l'utilisateur.
	int opt = 0;

	// Chaine de caractere du type de titre a ajouter en cas d'ajout.
	char* type_titre = NULL;

	// Variable d'etat d'operation de lecture du terminal.
	int status;

	// Variable de stockage de la taille de lecture du terminal.
	size_t taille;
	
	/*
	* Lab1
	* HLR01	: Detection des arguments provenant de l'utilisateur et gestion des
	* 			  erreurs d'entree des arguments.
	*/
	if ((argc < 3))
	{
		printf("ERROR: NO ARGUMENTS RECIEVED.\n");
		return EXIT_FAILURE;
	}
	if ((argc % 2 != 1))
	{
		printf("ERROR: ARGUMENT COUNT DOES NOT MATCH PAIR FORMATTING.\n");
		return EXIT_FAILURE;
	}
	if (argv == NULL)
	{
		printf("ERROR: NULL VALUED ARGUMENT ARRAY.\n");
		return EXIT_FAILURE;
	}
	for (int i = 0; i < argc; ++i){
		if (argv[0] == NULL)
		{
			printf("ERROR: NULL POINTER TO STRING IN POSITION %d.\n", i);
			return EXIT_FAILURE;
		}
	}

	// Creation de la structure des criteres du client.
	if(!(critere = critere_creer())){
		printf("ERREUR: IMPOSSIBLE DE CREER LE CRITERE.\n");
		return EXIT_FAILURE;
	}

	// Creation de la structure du type d'operation du client.
	if(!(operation = operation_creer()))
	{
		printf("ERREUR: IMPOSSIBLE DE CREER L'OPERATION.\n");
		return EXIT_FAILURE;
	}

	// Lecture des arguments donnees par l'utilisateur.
	/*
	 * @note   	HLR02 : Lecture des arguments -t, -p, -a et -r avec "getopt".
	 * @note	HLR03 : Lecture de l'argument -T pour le type d'operation voulu.
	 */
	while((opt = getopt(argc, argv, "T:t:r:a:p:")) != -1)
	{

		/*
		 * @note   	HLR07 : A partir des arguments recu, remplir la structure des
		 *					operations.
		 */
		// Gestion du type d'action requis.
		if(opt == CHAR_TYPE_OPERATION)
		{
			// Transformation des lettres minuscules en majuscule.
			if(strlen(optarg) == 1 && islower((int) optarg[0]))
			{
				optarg[0] -= (char) 32;
			}
			// Verification du type de commande recu.
			if(!((strlen(optarg) == 1) && 
				((optarg[0] == RECHERCHE) || 
				 (optarg[0] == RETRAIT) || 
				 (optarg[0] == AJOUT))))
			{
				printf("ERROR: ARGUMENT %s FOR '-%c' IS INVALID.\n", optarg,
					CHAR_TYPE_OPERATION);
				printf("TRY: -%c %c FOR SEARCH OR -%c %c FOR REMOVAL OR -%c %c TO ADD.\n",
					CHAR_TYPE_OPERATION, RECHERCHE, 
					CHAR_TYPE_OPERATION, RETRAIT, 
					CHAR_TYPE_OPERATION, AJOUT);
				return EXIT_FAILURE;
			}
			// Lecture de la commande et validation.
			else if(operation_set_t(operation, optarg[0]))
			{
				printf("ERROR: FAILED TO SET %c OPERATION %s TYPE\n", 
					CHAR_TYPE_OPERATION, optarg);
				return EXIT_FAILURE;
			}
		}
		
		// Gestion d'ajout des criteres dans la structure du premier laboratoire.
		else
		{
			if( !critere_set(critere, optarg, (t_tag_critere) opt))
			{
				printf("ERROR: FAILED TO SET A CRITERIA %c\n",opt);
				return EXIT_FAILURE;
			}
		}
	}

	/*
	 * @note   	HLR07 : A partir des criteres recu, emmagasiner la structure des criteres
	 *					dans la structure des operations. 
	 */

	// Une fois la lecture completee, ajouter les criteres a la structure des operations.
	operation_set_critere(operation, critere);

	/*
	 * @note   	HLR08 : Pour l'operation d'ajout, nous devons valider si tous les
	 *					arguments necessaires on ete recu, et ajouter le type de titre
	 *					aux criteres.
	 */
	if(operation_get_t(operation) == AJOUT)
	{
		// Verification de la presence des criteres titre et annee.
		if(operation_get_critere_i(operation, CHAR_TAG_TITRE, 0) == NULL)
		{
			printf("ERROR: ADD OPERATION REQUIRES MOVIE TITLE WITH '-t' COMMAND.\n");
			return EXIT_FAILURE;
		}
		else if(operation_get_critere_i(operation, CHAR_TAG_ANNEE, 0) == NULL)
		{
			printf("ERROR: ADD OPERATION REQUIRES MOVIE YEAR WITH '-a' COMMAND.\n");
			return EXIT_FAILURE;
		}

		// Boucle d'ecriture du type de titre par l'utilisateur.
		do
		{
			// Si les criteres sont valides, exiger le type de titre. Recommencer au besoin.
			printf("Please enter the title type to add this movie to the database.\nTitle type : ");

			// Reception de la ligne du terminal par "stdin".
			if((status = getline(&type_titre, &taille, stdin)) == -1)
			{
				free(type_titre);
				type_titre = NULL;
				printf("ERROR, CANNOT READ VALUE, PLEASE TRY AGAIN.\n");
			}

			// Si aucun texte n'est recu, indiquer l'erreur et recommencer.
			if(type_titre[0] == '\n')
			{
				status = -1;
				printf("ERROR, EMPTY STRING, PLEASE TRY AGAIN.\n");
			}

		} while(status == -1);

		// Copie de la ligne lue dans la structure d'operation.
		operation_set_type_titre(operation, type_titre);

		// Une fois termine avec la ligne de commande recu, liberer.
		free(type_titre);
	}

	#ifdef __DEBUG_PHASE_1

	printf("Successful reception from the user. The following is the received data :\n");

	//impression du critere
	imprimer_critere(operation_get_critere(operation));
	
	// Impression du type d'operation
	type_alias T = operation_get_t(operation);
	printf("Valeur de T :\n    %c\n", T);
	
	// Pour une operation d'ajout, imprimer le type de titre.
	if(T == AJOUT)
	{
		const char * tmp = operation_get_type_titre(operation);
		printf("Valeur du type de titre : \n    %s", (char*) tmp);
	}

	#endif	//__DEBUG_PHASE_1

	//------------------------------------------------------------------------------------

	/*				SECTION RESERVEE POUR LA COMMUNICATION AVEC LE SERVEUR 				*/

	//------------------------------------------------------------------------------------

	// Serialisation des donnees pour la communication avec le serveur.
	char* serial = serialiser_operation(operation, (cstr) STR_SEPARATEUR_CRITERES);

	// Liberation de la structure des opertions.
	operation_liberer(operation);
	
	#ifdef __DEBUG_PHASE_2

	printf("Donnees serie : \n%s\n", serial);

	pt_operation oper_deserie = deserialiser_operation((cstr) serial, (cstr) STR_SEPARATEUR_CRITERES);

	free(serial);

	//impression du critere
	imprimer_critere(operation_get_critere(oper_deserie));
	
	// Impression du type d'operation
	type_alias T_deserie = operation_get_t(oper_deserie);
	printf("Valeur de T :\n    %c\n", operation_get_t(oper_deserie));
	
	// Pour une operation d'ajout, imprimer le type de titre.
	if(T_deserie == AJOUT)
	{
		const char * tmp_deserie = operation_get_type_titre(oper_deserie);
		printf("Valeur du type de titre : \n    %s", (char*) tmp_deserie);
	}

	operation_liberer(oper_deserie);

	#endif	//__DEBUG_PHASE_2

	// Liste des packets de message a envoyer.
	h_tx_msg liste_msg;

	// Creation de la liste a partir des donnees recu.
	list_msg = tx_msg_creer_tx(getpid(),serial);

	t_comm_serveur data_thread_serveur;

	data_thread_serveur.liste_msg = list_msg;
	if(data_thread_serveur.pub_file = public_file_system_open() == NULL)
	{
		printf("ERROR, SERVER IS OFFLINE.\n");
		return EXIT_FAILURE;
	}

	// Creation du thread de communication avec le serveur.
	pthread_create(&thread_serveur, NULL, comm_serveur, &data_thread_serveur);

	// Pendant le thread, nous, on a du fun!

	// Les deux parametres du jeu sont la longueur et la largeur.
	tetris_run(12, 15);

	if(pthread_join(thread_serveur, NULL) != 0)
	{
		printf("ERROR PTHREAD.\n");
		exit(1);
	}
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void* comm_serveur(void* arg)
{
	// Traduction des donnees en argument du pthread pour manipulation.
	h_comm_serveur targ = (h_comm_serveur) arg;

	// Structure des packets a recevoir.
	t_packet tx;

	// list pop head (envoyer le premier element)?
	dir_node_lock(targ->pub_file->channels[RXID(0)], LOCK_EX, 0);

	dir_node_lock(targ->pub_file->channels[TXID(0)], LOCK_EX, 0);

	t_packet header;

	memcpy(&(header.tx_header), &(targ->liste_msg.tx_header), sizeof(t_tx_header));

	memset(&(header.data), 0, PACKET_DATA_LENGTH);

	// Envoie des donnees
	perrno_if_not_true(
		(0 == write(targ->channels[TXID(index)]->fd,header,sizeof(t_packet))),
		"message d'erreur%d",
		2
	);

	//--------------------------------------------------------------
	// 				Envoie des packets au serveur:
	//--------------------------------------------------------------

	// Structure des packets a recevoir.
	t_packet rx;

	// Handshake

	// Lecture du premier packet a recevoir, fonction bloquante.
	read(targ->channels[RXID(0)]->fd, &rx, sizeof(t_packet));

	// Index du canal de communication donnee par le serveur pour communiquer.
	int index = *((int*)rx.data);

	if(index <= 0)
	{
		targ->retval = NULL;
		exit(1);
	}

	// Reception des donnees du serveur.
	while(rx=list_pop_head(ls))
	{
		// Gestion des erreurs de reception.
		perrno_if_not_true(
			(0 == write(targ->channels[TXID(index)]->fd,tx,sizeof(t_packet))),
			"message d'erreur%d",
			2
		);

		// Gestion des packets recu...

		// Liberation des packets.
		free(rx);
	}

	//--------------------------------------------------------------
	// 				Reception des packets du serveur
	//--------------------------------------------------------------

}