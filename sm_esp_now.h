#ifndef sm_esp_now
#define sm_esp_now

#include <esp_now.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <Arduino.h>
#include "sm_mesures.h"
#include "sm_wifi.h"
#include "main.h"
#include <ArduinoJson.h>

const int maxValue = 10; // defini le nombre maximun de valeurs transmises par un capteur

// Structure example to receive data
// Must match the sender structure
// taille maximale = 250
// ici taille = 18 octets
typedef struct struct_message {
  uint8_t msgType; // 1 octet
  uint8_t id; // 1 octet
  unsigned int seqNum; // 4 octets -> utile ?
  float valeurs[maxValue]; // taille = 10 * 4 = 40 octets  -> on pourrait avoir float valeurs[60]
  // il reste 250 -46 soit 206 caractères 
  char stringValues[maxValue][maxStringLen] ; 
} struct_message;



typedef struct struct_pairing {       // new structure for pairing
    uint8_t msgType;
    uint8_t id;
    uint8_t macAddr[6];
    uint8_t channel;
} struct_pairing;


enum MessageType {PAIRING, DATA,};

bool setup_ESP_NOW(); // initialise le protocole de communication ESP-NOW de la carte 

#endif
