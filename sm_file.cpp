#include "sm_file.h"

bool SDstatus=false;

// Initialize SD_MMC card
bool setup_File(){
  //if(!serverBusy) SD_MMC.end();
  SDstatus=SD_MMC.begin("/sdcard", true);
  return SDstatus;
}

void print_File(char * path){
  File file = SD_MMC.open(path, FILE_READ);
  while(file.available()){
    Serial.println(file.readStringUntil('n'));
  }
  file.close();
}

String readHtml_File(char * filename){
  String _string = "";
  File file = SD_MMC.open(filename);
  while(file.available()){
    _string +=file.readString();
  }
  return _string;
}

bool testSDCard_File(){
  if(setup_File()){
    File file = SD_MMC.open("/config.txt");
    if (file.available()){
      String s = file.readStringUntil('\n');
      if(s.length()>0){
        file.close();
        SDstatus=true;
        return true;
      }     
    }
    file.close();
  }
  SDstatus=false;
  return SDstatus;
}
bool getSDstatus_File(){
	return SDstatus;
}
void append_File(const char * message) {
  File file = SD_MMC.open("/mesures.csv", FILE_APPEND);
  if(!file) {
    writeLog_File("Erreur d'ouverture de /mesures.csv");
    return;
  }
  file.print(message);
  file.close();
}
void writeLog_File(String message){
   File file = SD_MMC.open("/log.csv", FILE_APPEND);
   String _record = String(getCurrentTime_Wifi())+";"+timeToString_Wifi(getCurrentTime_Wifi())+";"+message;
   if(file){
     file.println(_record);
     file.close();
   }
}
