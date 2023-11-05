#ifndef sm_board
#define sm_board

#include <Arduino.h>
#include "sm_file.h"


// GESTION DES CAPTEURS
// Définition de la structure d'un capteur
typedef struct t_board {
  int boardId; // identifiant du capteur
  char * owner; // prénom du responsable du capteur
  char * mesureTypes; // les types de  mesure envoyées par le capteur (testé de 1 à 3)
  char * unites;
  char * titles;
  String  * payloads;
} board;

bool setup_Board(); // initialise la description des capteurs à partir du fichier boards.csv
char getBoardValueAsChar(board *_board, int index); // Retourne le type de mesure à la position définie par l'argument "index" du capteur board
int getMaxBoards(); // Retourne le nombre de capteurs qui ont été définis dans le fichier boards.csv
board * getBoardById(int boardId); // Retourne le capteur qui possède l'id défini par l'argument boardId
board * getBoardByIndex(int index); // Retourne le capteur à la position définie par l'argument index
String getIndex_html(); // Retourne la page html principale du tableau de bors des capteurs 
String getBoardString(String myString, int index); 
String boardToString (board _board); // Retourne une chaine de caractère représentant la description du capteur correspondant à l'argument _board
int getNumberOfValues( String data); // Retourne le nombre de valeurs définies dans le descripteur data 

#endif
