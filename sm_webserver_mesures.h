#ifndef sm_webserver_mesures
#define sm_webserver_mesures

#include "sm_file.h"
#include "sm_mesures.h"
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"

void resetServerBusy(); // Force l'état du serveur à prêt
AsyncWebServerResponse * fillMesures(AsyncWebServerRequest *request); // renvoie les mesures comprises entre la position définie par le paramètre "from" et celle définie par le paramètre "to" tous les portés par la requêtes /getMesures?from=xxx&to=yyy


#endif
