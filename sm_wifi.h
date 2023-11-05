#ifndef sm_wifi
#define sm_wifi

#include <WiFi.h>
#include <Arduino.h>
#include "TimeLib.h"

#include "sm_file.h"

bool setup_Wifi(); // initialise la connexion WiFi
bool isConnected_Wifi(); // Indique si le WiFi est connecté
String getBoxSsid_Wifi(); // retourne le ssid de la box qui donne l'accès à internet
int getAlarmLedPin_Wifi(); // Retourne le numéo de la pin qui indique l'éatt du réseau WiFi
int getChannel_Wifi(); // Retourne le numéro du canal utilisé par le WiFi
bool shouldBeConnected_Wifi(); // Indique si la carte devrait connectéeau WiFi
// interface pour la date et l'heure
unsigned long get_EpochTime_Wifi(); // Retourne la valeur de la date et de l'heure
void set_EpochTime_Wifi(unsigned long ep); // positionne la valeur de la date et de l'heure
unsigned long getCurrentTime_Wifi(); // Retourne la date et l'heure actuelle 
String timeToString_Wifi(unsigned long _t); // Retourne sous forme de chaine de caractères lisible par un humain la date et l'heure passée par l'argument t

#endif
