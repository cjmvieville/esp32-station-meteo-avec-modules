#include "sm_wifi.h"


const int alarmLedPin = 33;

String ssid = "";
String boxSsid = "";
String password = "";
String ntpServer = "pool.ntp.org";

struct tm timeinfo; // structure nécessaire pour stocker l'échange avec le serveur NTP
unsigned long epochTime; // pour stocker l'epoch/time en secondes du serveur NTP
long gmtOffsetSec;
int daylightOffsetSec; // UTILE ?

unsigned long get_EpochTime_Wifi(){
  return epochTime;
}
void set_EpochTime_Wifi(unsigned long ep){
  epochTime = ep;
}
// Retourne la date heure depuis le serveur NTP
unsigned long _getTime() {
  time_t now;
  if (!getLocalTime(&timeinfo)) {
    return(0);
  }
  time(&now);
  return (unsigned long)now;
}
// Retourne le temps réel 
unsigned long getCurrentTime_Wifi(){
  return epochTime+(millis()/1000UL);
}
// Retourne l'affichage de la date dans un format compréhensible par un humain
String timeToString_Wifi(unsigned long _t){
  char buff[32];
  unsigned long t=_t+gmtOffsetSec;
  sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", day(t), month(t), year(t), hour(t), minute(t), second(t));
  return String(buff);
}

String _getSsid(){
  return ssid;
}
String getBoxSsid_Wifi(){
  return boxSsid;
}
String _getPassword(){
  return password;
}
String _getNtpServer(){
  return ntpServer;
}
void _setSsid(String s){
  ssid=s;
}
void _setBoxSsid(String s){
  boxSsid=s;
}
void _setPassword(String s){
  password=s;
}
void _setNtpServer(String s){
  ntpServer=s;
}
int getChannel_Wifi(){
  return WiFi.channel();
}
bool shouldBeConnected_Wifi(){
  return boxSsid.length()>0;
}
int getAlarmLedPin_Wifi() {
  return alarmLedPin;                               
}
bool isConnected_Wifi(){
  return WiFi.status() == WL_CONNECTED ;
}
bool setup_Wifi(){
  File file = SD_MMC.open("/config.txt");
  if (file.available()){
    _setSsid(file.readStringUntil('\n'));
    _setBoxSsid(file.readStringUntil('\n'));
    _setPassword(file.readStringUntil('\n'));
    _setNtpServer(file.readStringUntil('\n'));
  }
  Serial.println("-----------------------");
  Serial.println(_getSsid());
  Serial.println(getBoxSsid_Wifi());
  Serial.println(_getPassword());
  Serial.println(_getNtpServer());
  Serial.println("-----------------------");
  file.close();
  pinMode(alarmLedPin, OUTPUT);
  digitalWrite(alarmLedPin, LOW);
  Serial.print("Server MAC Address:  ");
  Serial.println(WiFi.macAddress());
  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  // Set device as a Wi-Fi Station
  int nb=5;
  if(shouldBeConnected_Wifi()){
    WiFi.begin(getBoxSsid_Wifi().c_str(), _getPassword().c_str());
    while (nb>0 && WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
      delay(2000);
      WiFi.reconnect();
      Serial.println("Setting as a Wi-Fi Station...");
      nb--;
    }
  } 
  Serial.print("Server SOFT AP MAC Address:  ");
  Serial.println(WiFi.softAPmacAddress());

  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());
  WiFi.softAP(_getSsid().c_str(), "");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP); // normalement 192.168.4.1 c'est la valeur par défaut sur un ESP32
   // ATTENTE CONNEXION AU SERVEUR NTP SI 
  if(_getNtpServer().length()>0&&shouldBeConnected_Wifi()) {
  int essais=2;
  if(shouldBeConnected_Wifi()){
    configTime(gmtOffsetSec, daylightOffsetSec, _getNtpServer().c_str());
    while (essais>0&& epochTime == 0) {
      epochTime=_getTime();
      if(epochTime==0) {
        essais--;
        Serial.print("Le serveur NTP "+ _getNtpServer()); Serial.println(" n'a pas été correctement contacté. On attend 1 sec et on réessaye");
        writeLog_File("Le serveur NTP "+ _getNtpServer() +" n'a pas été correctement contacté. On attend 1 sec et on réessaye");
        delay(1000);
      }
    }
   }
  }
  
  if(epochTime>0&&_getNtpServer().length()>0){digitalWrite(alarmLedPin, HIGH);Serial.println("Le serveur NTP "+ _getNtpServer() + " a envoyé une valeur correcte");}
  else{Serial.println("Il faut connecter un navigateur à la station météo pour la mettre à l'heure");};
  Serial.println("epochTime : "+String(epochTime));
}
