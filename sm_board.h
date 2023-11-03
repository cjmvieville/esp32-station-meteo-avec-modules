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

bool setup_Board();
char getBoardValueAsChar(board *_board, int index);
int getMaxBoards();
board * getBoardById(int boardId);
board * getBoardByIndex(int index);
String getIndex_html();
String getBoardString(String myString, int index);
String boardToString (board _board);
int getNumberOfValues( String data);

#endif
