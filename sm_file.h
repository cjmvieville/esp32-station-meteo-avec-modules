#ifndef sm_file
#define sm_file

#include <Arduino.h>

#include "FS.h"
#include "SD_MMC.h" 
#include "SPI.h"
#include "sm_wifi.h"


bool setup_File(); // initialise l'acès à la carte SD
void print_File(char * path); 
String readHtml_File(char * filename); // Retourne le contenu du fichier défini par l'argument filename
bool testSDCard_File(); // Retourne l'état actuel de la carte SD (pour savoir si elle est opérationnelle)
bool getSDstatus_File(); // Retounr le dernier état connu de la carte SD
void append_File(const char * message) ; // ajoute le contenu de message à la fin du fichier mesures.csv
void writeLog_File(String message); // érit le message à la fin du fichier log.csv
#endif
