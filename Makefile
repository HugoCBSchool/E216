##########################################################################################
#	Fichier		: Makefile
#	Auteur		: Gabriel Gandubert
#	Date		: 11 mars 2021
#	Projet		: ELE216 - Laboratoire 2
#	institution	: Ecole de technologie superieure.
#
#	Description	: 	Ce fichier permet de compiler de facon efficace le programme
#					d'interaction avec une base de donnee de films dans le cadre
#					du cours ELE216
#
#	Actions "make" possible :
#
#								% Compilation du client et du serveur
#								compile_all (default)
#
#								% Compilation du serveur
#								app_serveur
#
#								% Compilation du client
#								app_client
#
#								% Nettoyage des objets produits par le Makefile. (reset)
#								clean
#
#
##########################################################################################



##########################################################################################
#								DECLARATION DES VARIABLES								 #
##########################################################################################

# Compilateur du programme
CC = gcc

# Parametres utilisees pour la compilation des modules.
CFLAGS = -g -Wall -pthread

# Fichier contenant les executables generees par le makefile.
EXEC_FOLDER = Build

# Fichier contenant les entetes .h du programme.
INCLUDE_FOLDER = inc

# Position des applications dans le dossier present.
SERVER_LOCATION = $(EXEC_FOLDER)/app_serveur
CLIENT_LOCATION = $(EXEC_FOLDER)/app_client

# Position du code a partir du dossier present et inclut.
MAIN_CLIENT_LOC = src/main_client.c
MAIN_SERVER_LOC = src/main_server.c
MODULES_LOC = src/modules/*.c
UTIL_LOC = src/util/*.c

# Local environnment variable name
ENV_VAR_NAME = DATABASE

##########################################################################################
#									APPELS SYSTEME										 #
##########################################################################################

# commande par defaut, on compile le programme du serveur et du client.
compile_all: app_serveur app_client

#----------------------------------------------------------------------------------------#
#								LANCEMENT DES EXECUTABLES								 #
#----------------------------------------------------------------------------------------#

# Lancement du serveur avant de lancer le client.
lancement_serveur: app_serveur
	Build/app_serveur

# Lancement du serveur avant de lancer le client.
lancement_client: lancement_serveur app_client
	Build/app_client

# Lancement du client pour debuggage.
lancement_test_client: app_client
	Build/app_client -T a -t titre1,titre2,titre3 -a 1998:2021 -p personne -r 1.1

# Declaration d'action .PHONY pour le lancement d'executable.
.PHONY: lancement_test_client lancement_client lancement_serveur test database_path

#----------------------------------------------------------------------------------------#
#								CREATION DES EXECUTABLES								 #
#----------------------------------------------------------------------------------------#

# Compilation du serveur.
app_serveur: modules util
	$(CC) $(CFLAGS) $(MAIN_SERVER_LOC) *.o -o $(SERVER_LOCATION) -I $(INCLUDE_FOLDER)

# Compilation du client.
app_client: util_client modules_client
	$(CC) $(CFLAGS) $(MAIN_CLIENT_LOC) *.o -o $(CLIENT_LOCATION) -I $(INCLUDE_FOLDER)

#----------------------------------------------------------------------------------------#
#								COMPILATION DES MODULES									 #
#----------------------------------------------------------------------------------------#

# Modules du programme
modules:
	$(CC) $(CFLAGS) -c $(MODULES_LOC) -I $(INCLUDE_FOLDER)

# Librairies utiles du programme.
util:
	$(CC) $(CFLAGS) -c $(UTIL_LOC) -I $(INCLUDE_FOLDER)

#----------------------------------------------------------------------------------------#
#								NETTOYAGE DU REPERTOIRE									 #
#----------------------------------------------------------------------------------------#

# Nettoyage de l'espace de travail.
clean: 
	rm -rf *.o
	rm -rf $(EXEC_FOLDER)/app_*


##########################################################################################
#								FONCTIONS TEMPORAIRES									 #
##########################################################################################

# Compilation du client.
app_test_client: util_client modules_client
	$(CC) $(CFLAGS) $(MAIN_CLIENT_LOC) client.o critere.o table_string.o fstr.o linklist.o -o $(CLIENT_LOCATION) -I $(INCLUDE_FOLDER)

# Fonction temporaire de compilation des modules du client
modules_client:
	$(CC) $(CFLAGS) -c src/modules/client.c src/modules/critere.c -I $(INCLUDE_FOLDER)

# Fonction temporaire de compilation des librairies utiles du client
util_client:
	$(CC) $(CFLAGS) -c src/util/fstr.c src/util/table_string.c src/util/linklist.c -I $(INCLUDE_FOLDER)

test: app_test_client
	./Build/app_client $(COMMAND)

database_path:
	source $(PATH)/Set_data_dir
	printenv DATABASE

##########################################################################################