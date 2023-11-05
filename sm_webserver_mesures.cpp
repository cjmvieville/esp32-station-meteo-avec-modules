#include "sm_webserver_mesures.h"

// les variables globales pour l'envoi des mesures stockées sur la carte
unsigned long _from = 1;
unsigned long  _to= 100;
unsigned long  numLigne=0;
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
    _from=getNextIdMesure()-1;
    _to=-1;
    if(request->hasParam("from")){
      _from=request->getParam("from")->value().toInt();
    }
    if(request->hasParam("to")){
      _to=request->getParam("to")->value().toInt();
    }
    if(_from<0){_from=0;}
    if(_to<0||_to<_from){ _to=-1;} 
    String s ="";

    while(file.available()&&numLigne<_from){
      s=file.readStringUntil('\n');
      pos+=s.length()+1;
      numLigne++;
    }
    if(_to==-1){
      len=0;
    } else {
      while(file.available()&&numLigne<=_to){
        s=file.readStringUntil('\n');
        numLigne++;
        len+=s.length()+1;
     } 
    }

    file.seek(pos);
    
    AsyncWebServerResponse *response = request->beginChunkedResponse("text/plain", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
      // il y a un to
      if (file.available()&&_to>0&&len>0){
        if(len<maxLen) {
          size_t l=len;
          len=0;
          size_t r= file.read(buffer, l);
          if(r==0) _close();
          return r;
        } else if(len>=maxLen){
          len-=maxLen;
          size_t r= file.read(buffer, maxLen);
          if(r==0) _close();
          return r;
        } 
      }
      // pas de to
      if (file.available()&&_to==-1&&len==0){
        size_t r= file.read(buffer, maxLen);
        if(r==0) _close();
        return r;
      }
     _close();
     return 0; 
      });
      return response;
}
