/*
  Claude Viéville
  Inspiré par https://RandomNerdTutorials.com/?s=esp-now
*/


#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"
#include <ArduinoJson.h>


#include "sm_file.h"
#include "sm_wifi.h"
#include "sm_esp_now.h"
#include "sm_mesures.h"
#include "sm_board.h"
#include "sm_webserver_mesures.h"


const int ledPin = 4; //n'est plus utilisée dans cette version (toujours éteinte)

bool sendEvents=false; // indique si le serveur doit envoyer des data aux navigateurs connectés



AsyncWebServer server(80); // le serveur web
AsyncEventSource events("/events"); // le gestionnaire d'envoi d'événements aux navigateurs

void sendWebEvents(String payload){
  events.send(payload.c_str(), "new_readings", millis());
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  setup_File();
  setup_Board();
  setup_Mesures();
  setup_Wifi();
  setup_ESP_NOW();
  // Start Web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", getIndex_html().c_str()); 
  });
  server.on("/sendEvents", HTTP_GET, [](AsyncWebServerRequest *request){
    sendEvents=true;
    request->send(200, "text/plain", "OK");
  });
  server.on("/getNbMesures", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(getNextIdMesure()));
  });
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    resetServerBusy();
    request->send(200, "text/plain", "serveur pret");
  });
  server.on("/setEpochTime", HTTP_GET, [](AsyncWebServerRequest *request){
    String s = "Le parametre epochTime absent"; 
    if(request->hasParam("epochtime")&&get_EpochTime_Wifi()==0){
      set_EpochTime_Wifi(strtol(request->getParam("epochtime")->value().c_str(),NULL, 10)-(millis()/1000UL)); 
      digitalWrite(getAlarmLedPin_Wifi(), HIGH);
    } else {
      request->send(200, "text/plain", s);
    }
    request->send(200, "text/plain", String(get_EpochTime_Wifi()+(millis()/1000UL)));
  });
  server.on("/getEpochTime", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", String(get_EpochTime_Wifi()+(millis()/1000UL)));
  });
  // Attention n'est pas terminée : il n'y a qu'une ébauche
  // actuellement le paramètre to n'est pas pris en compte
  server.on("/getMesures", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse * response = fillMesures(request);
    response->addHeader("Server","ESP Async Web Server");
    request->send(response);
  });
  

  // Events 
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    resetServerBusy();
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });

  server.addHandler(&events);
  
  // start server
  server.begin();
  Serial.print("nexIdMesure :");Serial.println(getNextIdMesure());
  Serial.println("fin setup()");
}

void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;

//  setChan_ESP_NOW(WiFi.channel());

  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    events.send("ping",NULL,millis());
    sendEvents=true;
    if (shouldBeConnected_Wifi()&&!isConnected_Wifi()) {
      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.reconnect();
    } 
    digitalWrite(getAlarmLedPin_Wifi(), (isConnected_Wifi() || !shouldBeConnected_Wifi()) && get_EpochTime_Wifi()>0); // si connecte  et à l'heure -> led eteinte
    lastEventTime = millis();   
  }
  if(testSDCard_File() && getListMesures()->length>0){
    storeMesure();
  }
  if(sendEvents){
    String payload;
    StaticJsonDocument<1000> carte;
    carte["time_stamp"] = getCurrentTime_Wifi();
    carte["SDstatus"] = getSDstatus_File()?"SD en place":"SD manquante";
    carte["nextMesureId"] = getNextIdMesure();
    carte["wifi"] = getBoxSsid_Wifi() + " : " + String(isConnected_Wifi()?"connecte":"non connecte");
    if(!shouldBeConnected_Wifi()) carte["wifi"] = "Aucune Box definie";
    carte["nbMesures"] = getListMesures()->length;
    serializeJson(carte, payload);
    sendWebEvents(payload);
    for(int i=0; i<getMaxBoards();i++){
      for(int j=0; j<getNumberOfValues(getBoardByIndex(i)->mesureTypes);j++){
        if(getBoardByIndex(i)->payloads[j].length()>0) sendWebEvents(getBoardByIndex(i)->payloads[j]);
      }
    }
    sendEvents=false;
  }
}
