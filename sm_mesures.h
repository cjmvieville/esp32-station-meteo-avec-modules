#ifndef sm_mesures
#define sm_mesures

#include <Arduino.h>

#include "sm_file.h"
#include "sm_board.h"
#include "sm_wifi.h"

// GESTION DE LA LISTE DES MESURES
// Définition de la structure d'une mesure
typedef struct t_mesure {
  int mesureId; // doit être unique si stockage BDD
  unsigned int seqNum;
  int boardId; // l'id de la carte
  char typeId; //identifiant du type de mesure
  long timeStamp; // estampille temporelle
  float value; // valeur de la mesure
} mesure;
// Définition de la structure d'un noeud de la liste
struct t_mesureNode{
  t_mesure * p_mesure;
  struct t_mesureNode * p_next;
};
//définition de la liste elle-même
typedef struct t_listeMesures{
    size_t length;
    struct t_mesureNode *p_head; // première mesure de la liste
    struct t_mesureNode *p_last; // derniere mesure de la liste
    struct t_mesureNode *p_display; // dernière mesure affichée
} ListeMesures;

bool setup_Mesures();
unsigned long getNextIdMesure();
mesure* creerMesure(board *_board, unsigned int seqNum, float value, int index);
ListeMesures * getListMesures();
void ajouterMesure(mesure * p_mesure);
void storeMesure();
#endif
