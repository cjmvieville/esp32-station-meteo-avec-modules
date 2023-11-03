#ifndef sm_webserver_mesures
#define sm_webserver_mesures

#include "sm_file.h"
#include "sm_mesures.h"
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"

void resetServerBusy();
AsyncWebServerResponse * fillMesures(AsyncWebServerRequest *request);

#endif
