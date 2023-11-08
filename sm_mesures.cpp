#include "sm_mesures.h"

// Function utilisée pour créer la liste des mesures
ListeMesures * _creerListeMesures(void){
   ListeMesures * p_new = (ListeMesures *)malloc(sizeof (t_listeMesures));
   if(p_new != NULL) {
     p_new->length=0;
     p_new->p_head=NULL;
     p_new->p_last=NULL;
  }
  return p_new;
}
ListeMesures * listeMesures=_creerListeMesures();

// Retourne le label d'une mesure 
char _getTypeId(mesure * p_mesure){
  return p_mesure->typeId;
}
// Retourne la valeur d'une mesure 
float _getValue(mesure * p_mesure){
  return p_mesure->value;
}
// Retourne la stringValue d'une mesure 
char *_getStringValue(mesure * p_mesure){
  return p_mesure->stringValue;
}
// Retourne l'estampille temporelle d'une mesure
long _getTimeStamp(mesure * p_mesure){
  return p_mesure->timeStamp;
}

// Retourne l'identifiant de la mesure
int _getMesureId(mesure * p_mesure){
  return p_mesure->mesureId;
}
unsigned long nextIdMesure = 0; // pour attriber un identifiant à chaque mesure (cette variable sera initialisée par la valeur stockée dans les préférences)

unsigned long getNextIdMesure(){
	return nextIdMesure;
}
void setNextIdMesure(unsigned long nim){
	nextIdMesure=nim;
}
void _incIdMesure(){
	nextIdMesure++;
}
/* Fonction pour créer une mesure 
 *  En entrée :
 *  - _board la carte ayant envoyée la mesure
 *  - la valeur de la mesure
 *  - l'index de cette mesure pour cette carte (0-2)
 */
mesure* creerMesure(board *_board, unsigned int seqNum, float value, char stringValue[], int index){
  mesure* _mesure = (mesure *) malloc(sizeof (* _mesure));
  if(_mesure!=NULL){
    _mesure->mesureId=nextIdMesure++;
    _mesure->boardId=_board->boardId;
    _mesure->seqNum=seqNum;
    _mesure->typeId=getBoardValueAsChar(_board,index);
    _mesure->timeStamp=getCurrentTime_Wifi();
    _mesure->value=value;
    memcpy(&_mesure->stringValue,stringValue,maxStringLen);
    return _mesure;
  }
  return NULL;
}
//////////////////////////////////////////////////////
// FONCTIONS POUR AFFICHER UNE MESURE

// Retourne une string décrivant la mesure
String _mesureToString(mesure * p_mesure){
  return "Mesure reçue de "+ String(p_mesure->boardId) + " ["+ getBoardById(p_mesure->boardId)->owner + "] ---> " +String(_getMesureId(p_mesure)) + " à " + String(_getTimeStamp(p_mesure)) + " secs depuis 1/1/1970 : " + String(_getTypeId(p_mesure)) + " = " + String(_getValue(p_mesure))+" ("+String(_getStringValue(p_mesure))+")";
}


String _mesureToCSV(mesure * _mesure){
  String _s ="";
  _s+=String(_mesure->mesureId)+";";
  _s+=String(_mesure->boardId)+";";
  _s+=String(_mesure->typeId)+";";
  _s+=String(_mesure->timeStamp)+";";
  _s+=String(timeToString_Wifi(_mesure->timeStamp))+";";
  _s+=String(_mesure->value)+";";
  _s+=String(_mesure->stringValue)+"\n";
  return _s;
}


int _initNextMesureId(){
  setNextIdMesure(0);
  if(!setup_File()){
    return 0;
  }
  File file = SD_MMC.open("/mesures.csv", FILE_READ);
  while(file.available()){
    file.readStringUntil('\n');
    _incIdMesure();
  }
  file.close();  
  return getNextIdMesure();
}
bool setup_Mesures(){
  return _initNextMesureId()>0;
}

board * _getBoard(mesure * p_mesure){
  return getBoardById(p_mesure->boardId);
}
String _readMesures(int from, int to){
  String s ="";
  int nb=1;
  if(!setup_File()&&from>to){
    return "Donnees non accessibles";
  }
  File file = SD_MMC.open("/mesures.csv", FILE_READ);
  while(file.available()&&nb<from){
    file.readStringUntil('\n');
    nb++;
    delay(1);
  }  
  while(file.available()&&(to==-1||nb<=to)){
    s+=file.readStringUntil('\n');
    s+="\n";
    nb++;
    delay(1);
  }
  file.close();  
  return s;
}
String _readNbMesures(){
  if(!setup_File()){
    return "Donnees non accessibles";
  }
  int nb=0;
  File file = SD_MMC.open("/mesures.csv", FILE_READ);
  while(file.available()){
    file.readStringUntil('\n');
    nb++;
    delay(1);
  }
  file.close();  
  return String(nb);
}

ListeMesures * getListMesures(){
  return listeMesures;
}
////////////////////////////////////////////////////////////////
// Fonctions pour gérer la  liste
/*
 * Ajoute une mesure à la fin de la liste
 */
void ajouterMesure(mesure * p_mesure){

  struct t_mesureNode * p_new = (t_mesureNode * )malloc(sizeof * p_new);
  if(p_new != NULL) {
    p_new->p_mesure=p_mesure;
    p_new->p_next=NULL;
  }
  
  if (listeMesures->p_head == NULL) {
     // la liste est vide 
    listeMesures->p_head = p_new; 
  } else {
    // la liste n'est pas vide, on ajoute la mesure à la fin
    listeMesures->p_last->p_next = p_new;
  }
  listeMesures->p_last = p_new;
  listeMesures->length++; /* Incrémentation de la taille de la liste */

}
void storeMesure(){
  t_mesureNode * node = listeMesures->p_head;
  listeMesures->p_head = node->p_next;
  listeMesures->length--; /* Incrémentation de la taille de la liste */
  if(listeMesures->length==0){
    listeMesures->p_last = NULL;
    listeMesures->p_head = NULL;
  }
  append_File((char *)_mesureToCSV(node->p_mesure).c_str());
  Serial.println("********** ENLEVER********************");
  Serial.println(_mesureToString(node->p_mesure));
  free(node->p_mesure); 
  free(node); 

}
