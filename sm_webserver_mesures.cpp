#include "sm_webserver_mesures.h"

// les variables globales pour l'envoi des mesures stockées sur la carte
int from = 1;
int to= 100;
int numLigne=0;
File file;
bool serverBusy=false;

void resetServerBusy(){
  serverBusy=false;
}
AsyncWebServerResponse * fillMesures(AsyncWebServerRequest *request){
if(serverBusy){
      request->send(503, "text/plain", "serveur occupe");
    }
    serverBusy=true;
    setup_File();
    file = SD_MMC.open("/mesures.csv", FILE_READ);
    numLigne=0;
    from=getNextIdMesure()-1;
    to=-1;
    if(request->hasParam("from")){
      from=request->getParam("from")->value().toInt();
    }
    if(request->hasParam("to")){
      to=request->getParam("to")->value().toInt();
    }
    if(from<0){from=0;}
    if(to<0||to<from){ to=-1;}  

    AsyncWebServerResponse *response = request->beginChunkedResponse("text/plain", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
      while(file.available()&&numLigne<from){
        file.readStringUntil('\n');numLigne++;
      }
      if (file.available()){
        return file.read(buffer, maxLen);  
        } else {
          file.close();
          serverBusy=false;
          return 0;
        }
      /*if(file.available()&&numLigne<from){
        if(numLigne<nextIdMesure-10000){for (int i=0; i<10000; i++){file.readStringUntil('\n');numLigne++;}}
        if(numLigne<nextIdMesure-1000){for (int i=0; i<1000; i++){file.readStringUntil('\n');numLigne++;}}
        if(numLigne<nextIdMesure-100){for (int i=0; i<100; i++){file.readStringUntil('\n');numLigne++;}}
        if(numLigne<nextIdMesure-10){for (int i=0; i<10; i++){file.readStringUntil('\n');numLigne++;}}
        file.readStringUntil('\n');
        numLigne++;
        memcpy(buffer, s.c_str(), s.length()+1);
        return s.length();
        if (imageSize - alreadySent >= maxLen) {
      memcpy(buffer, jpg + alreadySent, maxLen);
      return maxLen;
    } else {  // last chunk and then 0
      memcpy(buffer, jpg + alreadySent, imageSize - alreadySent);
      return imageSize - alreadySent; 
    }
      } */
     /* while(file.available()&&numLigne<from){
        file.readStringUntil('\n');numLigne++;
        if(numLigne%100==0){
            memcpy(buffer, s.c_str(), s.length()+1);
            return s.length()+1;
        }
      }
      
      s="\n";
      if(file.available()&&(to==-1||numLigne<=to)){
        s+=file.readStringUntil('\n');
        numLigne++;
        memcpy(buffer, s.c_str(), s.length()+1);
        return s.length()+1;
      } 
      file.close();  
      serverBusy=false;
      return 0;*/
      });
      return response;
}
