#include "sm_webserver_mesures.h"

// les variables globales pour l'envoi des mesures stockées sur la carte
int from = 1;
int to= 100;
int numLigne=0;
size_t len=0;
size_t pos=0;
File file;
bool serverBusy=false;

void resetServerBusy(){
  serverBusy=false;
}
size_t _close(){
  file.close();
  serverBusy=false;
}
AsyncWebServerResponse * fillMesures(AsyncWebServerRequest *request){
if(serverBusy){
      request->send(503, "text/plain", "serveur occupe");
    }
    serverBusy=true;
    len=0;
    pos=0;
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
    String s ="";


    while(file.available()&&numLigne<from){
      s=file.readStringUntil('\n');
      pos+=s.length()+1;
      numLigne++;
    }
    if(to!=-1){
      while(file.available()&&numLigne<=to){
        s=file.readStringUntil('\n');
        numLigne++;
        len+=s.length()+1;
     } 
    } else {
      len=-1;
    }

    Serial.print("from= ");Serial.println(from);
    Serial.print("to= ");Serial.println(to);
    Serial.print("len= ");Serial.println(len);
    Serial.print("pos= ");Serial.println(pos);
    file.seek(pos);
    
    AsyncWebServerResponse *response = request->beginChunkedResponse("text/plain", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
      // il y a un to
      if (file.available()&&len>=0){
        Serial.print("len= ");Serial.println(len);
        if(len<maxLen) {
          size_t l=len;
          len=0;
          size_t r= file.read(buffer, l);
          if(r==0) _close();
          return r;
        } else if(len>=maxLen){
          len-=maxLen;
          if(len<0) len=0;
          size_t r= file.read(buffer, maxLen);
          if(r==0) _close();
          return r;
        } 
      }
      // pas de to
      if (file.available()&&len<0){
        size_t r= file.read(buffer, maxLen);
        if(r==0) _close();
        return r;
      }

      
      });
      return response;
}
