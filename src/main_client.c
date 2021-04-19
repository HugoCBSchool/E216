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


#include "base.h"
// Librairires standard Pour la communication (Phase 2)

#include "tetris.h"

// Liste des modules necessaire pour ce programme.
#include "table_string.h"
#include "client.h"
#include "debug_include.h"


#include "buffer.h"
#include "networking.h"
#include "header.h"


// Appel des librairies necessaire pour ce programme.

#define MAX_EVENTS 256
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                           Fonctions locales                                      */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


//------------------------------------------------------------------------------------

/**
 * @brief  Appelee par le thread de gestion de la communication avec le serveur.
 * @param  arg	: les arguments provenant du pthread_create();.
 * @retval None
 */
void* thread_com_task(void* arg);


/**
 * @brief  lance tetris et cest tout
 * @param  argc: 
 * @retval 
 */
void* thread_tetris_task(void* unused);
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                           Fonction main                                          */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
int main(int argc, char **argv){
	//cx-HLR01

	// Thread pour la communication avec le serveur.
	pthread_t 		thread_com;
	pthread_t   	thread_tetris;
	pt_operation	operation	= NULL;
	str				serial 		= NULL;
	h_Buffer		buf;
	unsigned int	isReplyPrintable=0;
	{	//argument parsing:

		// Variables des structures des commandes/arguments de l'utilisateur.
		pt_critere		critere		= NULL;

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
		/**
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
				// Verification du type de commande recu.
				if(!(	(strlen(optarg) == 1) 	&& 
						isdigit(optarg[0]) 		&& 
						(atoi(optarg) <= AJOUT)
				)){
					printf(
						"ERROR: ARGUMENT %s FOR '-T' IS INVALID.\n"
						"TRY: -T %d FOR SEARCH OR -T %d FOR REMOVAL OR -T %d TO ADD.\n",
						optarg,
						RECHERCHE, 
						RETRAIT, 
						AJOUT
					);
					
					return EXIT_FAILURE;
				}
				// Lecture de la commande et validation.
				else if(operation_set_t(operation, atoi(optarg)))
				{
					printf("ERROR: FAILED TO SET T OPERATION %s TYPE\n", optarg);
					// Transformation des lettres minuscules en majuscule.
					if(strlen(optarg) == 1 && islower((int) optarg[0]))
					{
						optarg[0] -= (char) 32;
					}
					// Verification du type de commande recu.
					if(!((strlen(optarg) == 1) && 
						((optarg[0] == RECHERCHE) || 
						(optarg[0] == RETRAIT) || 
						(optarg[0] == AJOUT)))
					){
						printf("ERROR: ARGUMENT %s FOR '-%c' IS INVALID.\n", optarg,
							CHAR_TYPE_OPERATION);
						printf(
							"TRY: -%c %c FOR SEARCH OR -%c %c FOR REMOVAL "
							"OR -%c %c TO ADD.\n",
							CHAR_TYPE_OPERATION, RECHERCHE, 
							CHAR_TYPE_OPERATION, RETRAIT, 
							CHAR_TYPE_OPERATION, AJOUT
						);
						return EXIT_FAILURE;
					}
					// Lecture de la commande et validation.
					else if(operation_set_t(operation, optarg[0]))
					{	
						/*
						* @note   COM:HLR01
						* @note   COM:HLR05
						* @note   COM:HLR09        
						*/
						printf("ERROR: FAILED TO SET %c OPERATION %s TYPE\n", 
							CHAR_TYPE_OPERATION, optarg);
						return EXIT_FAILURE;
					}
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

		/**
		* @note  HLR07 : A partir des criteres recu, emmagasiner la structure des criteres
		*					dans la structure des operations. 
		*/

		// Une fois la lecture completee
		//ajouter les criteres a la structure des operations.
		operation_set_critere(operation, critere);

		/**
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
				// Si les criteres sont valides, exiger le type de titre. 
				//Recommencer au besoin.
				printf(
					"Please enter the title type to add this"
					" movie to the database.\nTitle type : "
				);

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

			}
			while(status == -1);

			// Copie de la ligne lue dans la structure d'operation.
			operation_set_type_titre(operation, type_titre);

			// Une fois termine avec la ligne de commande recu, liberer.
			free(type_titre);
		}
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


	// Serialisation des donnees pour la communication avec le serveur.
	if(!(serial = serialiser_operation(operation))){
		printf("FATAL ERROR WHILE INITIALIZING COMMUNICATION DATA.\n");
		operation_liberer(operation);
		return EXIT_FAILURE;
	}
	isReplyPrintable=(operation->T==RECHERCHE);
	// Liberation de la structure des opertions.
	operation_liberer(operation);
	
	#ifdef __DEBUG_PHASE_2
	{
		printf("Donnees serie : \n%s\n", serial);

		pt_operation oper_deserie = deserialiser_operation(
			(cstr) serial, 
			(cstr) STR_SEPARATEUR_CRITERES
		);

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
	}
	#endif	//__DEBUG_PHASE_2

	{//init worker and launch
		buf=BufferNew((strlen(serial)+sizeof(CommHeader))*sizeof(char));
		CommHeader header={.sentinel=SENTINEL,.size=strlen(serial)};
		memcpy(buf->buf_head,&header,sizeof(CommHeader)*sizeof(char));
		buf->csor_rd+=sizeof(CommHeader);
		strcpy(buf->csor_rd,serial);
		buf->csor_rd+=strlen(buf->csor_rd);

		free(serial);

		if(pthread_create(&thread_com,NULL,thread_com_task,&buf)!=0){
			printf("FATAL ERROR OCCURED WHEN STARTING COMMUNICATION WITH SERVER.\n");
			BufferFree(buf);
			return EXIT_FAILURE;
		}
	}

	{ //init the tetris game and launch
		if(pthread_create(&thread_tetris,NULL,thread_tetris_task,NULL)!=0){
			
			printf(
				"ERROR OCCURED WHEN INITIALIZING WAITING ROOM'S GAME.\n"
				"THE COMMUNICATION WILL CONTINUE UNTIL ITS EXIT POINT IS REACHED\n"
			);
		}
	}

	{//join point
		if(pthread_join(thread_com,NULL)==-1){
			if(pthread_cancel(thread_tetris)!=0){
				printf(
					"FATAL ERROR IN COMMUNICATION HANDLER."
					"THE COMMUNICATION HAS BEEN LOST.\n"
					"RESTARTING THE APPLICATION IS REQUIRED TO ENSURE RECOVERY.\n"
				);
			}

			return EXIT_FAILURE;
		}
		if(!buf){
			if(pthread_cancel(thread_tetris)==0){
				errno=ECOMM;
				perrno("%s","COMMUNICATION WITH SERVER TERMINATED DUE TO ERROR.\n");
			}
			return EXIT_FAILURE;
		}
		if(pthread_cancel(thread_tetris)!=0){
			return EXIT_FAILURE;
		}
	}	

	do{//result construction and output
		/**
		* @note   COM:HLR08
		* @note   COM:HLR04
		* @note   COM:HLR13
		*/
		h_CommHeader header=(h_CommHeader)buf->buf_head;
		CommHeaderMarshall(header);
		buf->csor_rd=buf->buf_head+sizeof(CommHeader)*sizeof(char);
		if(!header->size){
			printf("Request not completed. Error on the server side.\n");
			break;
		}
		if(!isReplyPrintable){
			printf("Operation completed succesfully\n");
			break;
		}
		printf("RESULTS READY. Results will be printed to console.\n");
		printf("%s",buf->csor_rd);
		break;
	}while(0);

	BufferFree(buf);

	printf(
		"Operation complete.\n"
		"Exiting...\n"
	);

	return EXIT_SUCCESS;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void* thread_com_task(void* arg){
	// Traduction des donnees en argument du pthread pour manipulation.
	h_Buffer buf	=Cast(Buffer,arg);
	sockFD   server	=SocketConnect(SERVER_IP,PORT_NUM);

	switch(BufferIO_send(buf,server)){
		case -1: 
			((h_CommHeader)buf->buf_head)->size=0;
			close(server);
			return NULL;
		case 0:
			((h_CommHeader)buf->buf_head)->size=0;
			close(server);
			return NULL;
		default:
			break;
	}
	free(BufferMove(buf));
	BufferInit(buf,sizeof(CommHeader));
	if(BufferIO_recv(buf,server)<=0){
		((h_CommHeader)buf->buf_head)->size=0;
		close(server);
		return NULL;
	}
	h_CommHeader header=(h_CommHeader)buf->buf_head;
	uint64_t sz;
	CommHeaderMarshall(header);
	if((sz=header->size)<=1){
		close(server);
		return buf;
	}

	BufferInit(buf,(sizeof(CommHeader)+sz)*sizeof(char));
	((h_CommHeader)buf->buf_head)->sentinel=SENTINEL;
	((h_CommHeader)buf->buf_head)->size=sz;
	buf->csor_rd=buf->buf_head+sizeof(CommHeader);

	if(BufferIO_recv(buf,server)<sz){
		((h_CommHeader)buf->buf_head)->size=0;
		close(server);
		return buf;
	}
	close(server);
	return buf;
}
/**
 * @brief  fonction de type pthread_cleanup pour ajouter au kill queue
 * @note   RTFM
 */
static void tetris_thread_cleanup(void* arg){
	tetris_signal_quit(0);
}

/**
 * @brief  thread roulant tetris
 */
void* thread_tetris_task(void* unused){
	//on push le cleanup sur le cancellation cleanup stack
	pthread_cleanup_push(tetris_thread_cleanup,NULL);
	tetris_run(12,15);
	//on pop la tache et l'execute
	pthread_cleanup_pop(1);
	return NULL;
}

