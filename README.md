# PROJET : ELE216 - LABORATOIRE 2

Développeurs :

 - Gabriel Gandubert
 - Hugo Cusson-Bouthillier

Date de remise : 17 mars 2021

Institution : École de technologie supérieure, Mtl

plateforme de développement : Eclipse 2020-06

Système d'exploitation : Linux

Ce document présente le fonctionnment, l'installation, la compilation, l'utilisation des 
exécutables, les résultats attendus, et la désinstallation du programme. Ce programme à été 
créé dans le cadre du cours ELE216 de l'ÉTS à la session d'hiver 2021.

# TABLE DES MATIÈRES

[[_TOC_]]

# INTRODUCTION

Ce projet permet de communiquer d'une application client vers une application serveur 
pour effectuer des opérations sur une base de données de films. Les opérations possibles sont :

 - Recherche
 - Ajout
 - Retrait

Ces opérations peuvent être utilisées par l'utilisateur au moment du lancement de l'application. 
L'application ne supporte qu'une opération a la fois. Pour plusieurs opérations différentes, il 
faut appeler l'application plusieurs fois. Une application "client" est utilisée pour traiter la 
demande de l'utilisateur. A partir de ce programme, une formulation de demande est vérifiée selon 
les paramètres fournis lors du lancement de l'exécutable client. Les paramètres sont utilisées 
pour identifier les éléments de la base de données visée par l'opération. Les paramètres sont les 
suivantes :

 - Le type d'opération a effectuer
 - Le ou les titre(s) de film(s).
 - L'année de parution du ou des film(s) (avec ou sans interval)
 - Le nom d'un ou des acteur(s) du ou des film(s)
 - La ou les cote(s) du film

Ces paramètres permettent de trouver les éléments les plus pertinents a votre recherche 
présent dans la base de données. Pour toutes les opérations, un titre est requis. Dans 
le cas specifique d'ajout d'un élément a la base de données, trois paramètres sont requis. 
Ces paramètres sont le titre du film et l'annee de parution. Une fois les deux paramètres 
validees, le programme va exiger un type de titre, ce qui correspond a son genre: Action, 
Drame, Comedie, etc. Les résultats de recherches sont écrit dans des fichiers textes pour 
Les titres, les acteurs et les cotes correspondantes. Si, par exemple, vous n'avez pas 
demandé de cote, le fichier des résultats des cotes ne sera pas créé ou modifié. Les 
résultats combinées de ces trois bases de données seront affichées au client et écrits dans 
un fichier texte. Pour une operation d'ajout ou de retrait, une confirmation du serveur sera 
envoyée.

# PROCÉDURE D'INSTALLATION

## Préparation de l'environnement pour le serveur

Afin que le serveur puisse connaître la position des bases de données sur le système, quelques 
étapes doivent être suivi :
1. En téléchargeant les données du projet sur votre ordinateur, un script nommée *Script_setup_database* 
 sera inclut. Ce script doit être déplacé vers les bases de données et exécutée dans le terminal avec 
 la ligne de commande suivante :
 
 `...$ ./Script_setup_database`<shell>

2. Copier la première ligne **complète** correspondant au chemin (path) vers le dossier présent ainsi 
 qu'une variable d'environnement associé avec le serveur. Pour copier à partir du *shell*, utilisez Ctrl+Shift+C. 
 Il y a deux choix pour installer la variable d'environnement (aussi décrits par le script):
    
    2.1. AJOUT TEMPORAIRE: En ajoutant une variable d'environnement globale *temporaire*, cette variable ne sera 
    utilisable que par le terminal qui a exécuté la commande. Ce faisant, ce terminal devra rester ouvert et 
    sera responsable de l'exécution du serveur. Cette variable est rapide à installer. Suivre l'étape **3.** pour
    installer ce type de variable. Voir l'étape **8.** pour valider la présence de la variable d'environnement.
    
    2.2. AJOUT À LONG TERME[^1]: En ajoutant une variable d'environnement globale *statique*, cette variable sera
    exécutable par tous les terminaux ouverts en tout temps. La procédure d'installation de cette variable prends 
    plus de temps. Suivre les étapes **4.** à **7.** afin de l'installer. Voir l'étape **8.** pour 
    valider la présence de la variable d'environnement.

3. AJOUT TEMPORAIRE: Coller la ligne directement dans le terminal désirée (peu importe le *path* du terminal à ce 
 moment) et exécutez la commande. La ligne devrait ressembler à: 

 `...$ export DATABASE=/home/.../<position base de données>`<shell>
 
4. AJOUT À LONG TERME[^1]: Ouvrez le fichier des variables d'environnement .profile (ou un fichier qui est exécuté par tous
 les terminaux ouverts tel que bashrc) présent sous ~/.profile. Un exemple est donné avec l'application nano :

 `...$ nano ~/.profile`<shell>

5. Coller la commande **à la fin du fichier** ouvert.

6. Sauvegarder et fermer le fichier.

7. Fermer la session en cours (logout) et réouvrez la session. La variable sera compilée et accessible.

8. Entrez la ligne suivante pour valider la présence de la variable d'environnement.
 
 `...$ printenv DATABASE`<shell>

## Compilation

Pour compiler le serveur et le client, il est possible d'utiliser Eclipse ou le terminal
Linux. 

### Compilation par Eclipse

Pour une compilation dans Eclipse, nous devons sélectionner le fichier **inc** comme étant
le fichier le reference pour les fichiers .h a inclure lors de la compilation. 

### Compilation par le terminal Linux

Pour compiler le serveur et le client, nous pouvons appeler directement la commande **make** :

`.../lab2$ make`<shell>

Il est aussi possible de compiler le serveur et le client de facon indépendante en utilisant les commandes **make app_serveur** et **make app_client**

`.../lab2$ make app_serveur`<shell>

`.../lab2$ make app_client`<shell>

Pour compiler le serveur et le client, et exécuter le serveur, nous pouvons utiliser la commande **make lancement_serveur** :

`.../lab2$ make lancement_serveur`<shell>

A la fin de l'utilisation de l'application, nous pouvons utiliser la commande **make clean** pour effacer les fichiers objets
utilisées par le Makefile pour la compilation et effacer les exécutables (serveur et client) produits par la compilation. 
Cette action doit etre utilisée uniquement a la fin de l'utilisation du programme.

`.../lab2$ make clean`<shell>

# PROCÉDURE D'UTILISATION

## Criteres minimaux avant utilisation (setup).

Pour utiliser le programme, deux conditions doivent être respectées:
 1. La compilation du serveur et du client est complète.
 2. Le serveur est actif et attends les données du client.

Une methode simple pour rencontrer ces deux criteres est d'utiliser la commande **make lancement_serveur** (voir la reference plus haut) pour compiler les applications et lancer le serveur.

## Utilisation de l'exécutable

### Syntax d'exécution de l'application client

Pour utiliser le programme client, nous devons utiliser l'exécutable généré apres la compilation. Cet exécutable est diponible dans le fichier Build après la compilation. Cet exécutable est appelé de la façon suivante:

`.../lab2/Build$ app_client -T <operation> -t <titre(s)> -p <personne(s)> -a <année(s)> -r <cote(s)>`<shell>

Note: Les paramètres peuvent être écrits dans n'importe quel ordre, du temps que l'operateur est suivit du contenu associé. Par exemple :

`.../lab2/Build$ app_client -r <cote(s)> -p <personne(s)> -T <operation> -a <année(s)> -t <titre(s)>`<shell>

Est une exécution valide.

### Paramètres acceptées par l'application client

 - **-T** : Accepte trois caractère selon l'operation : **'S'** pour **"Search"** ou **Recherche**, **'A'** pour **"Add"** ou 
  **Ajout** et **'R'** pour **"Remove"** ou **Retrait**.
  Une seule lettre peut être prise à la fois et **les lettres peuvent être en majuscule ou en minuscule**.
 
 - **-t** : Accepte un ou plusieurs titres séparés par des virgules. Si vous ajoutez des espaces dans le titre du film, ajouter
 les guillemets anglais **""** sur l'ensemble du paramètre.

 - **-p** : Accepte un ou plusieurs noms séparés par des virgules. Si vous ajoutez des espaces dans le nom des acteurs (nom et 
 nom de famille), ajouter les guillemets anglais **""** sur l'ensemble du paramètre.

 - **-a** : Accepte un ou plusieurs années séparées par des virgules. Si vous souhaitez une interval d'année (pour une recherche), 
 utiliser les deux points **":"** pour séparer les deux années de l'interval. Aucun espace ne devrait être inclut dans ce paramètre.

 - **-r** : Accepte un ou plusieurs cotes séparées par des virgules. Aucun espace ne devrait être inclut dans ce paramètre.

### Pendant l'exécution

Lors de l'exécution du programme, le serveur prendra un certain temps pour effectuer l'opération désirée. Pendant
ce temps, vous pouvez jouer au jeu Tetris directement dans le terminal. Les contrôles pour jouer au jeu sont les
suivantes:

    Bouger le bloc vers la gauche: a ou flèche gauche
    Bouger le bloc vers le bas (descente rapide): s ou flèche bas
    Bouger le bloc vers la droite: d ou flèche droite
    Tourner le bloc: [barre d'espacement] ou w ou flèche haut

Le projet original du jeu Tetris en ASCII provient de la source suivante:
[Programme Tetris original](https://github.com/Gregwar/ASCII-Tetris)

### Résultats attendus à la fin de l'exécution

 - **Pour une opération de recherche :** L'application client

[^1]: Utilisation du terme *Ajout permanent* aurait également été approprié, mais puisqu'il est possible de retirer
la variable d'environnement après ce type d'ajout, le terme *permanent* semble sous-entendre qu'elle sera présente
pour toujours.
