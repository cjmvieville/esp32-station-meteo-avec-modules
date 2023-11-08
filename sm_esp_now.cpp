#include "sm_esp_now.h"


esp_now_peer_info_t slave;
//int chan; 
struct_message incomingReadings;
struct_message outgoingSetpoints;
struct_pairing pairingData;

MessageType messageType;


// ---------------------------- esp_ now -------------------------
void _printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
}

bool _addPeer(const uint8_t *peer_addr) {      // add pairing
  memset(&slave, 0, sizeof(slave));
  const esp_now_peer_info_t *peer = &slave;
  memcpy(slave.peer_addr, peer_addr, 6);  
  slave.channel = getChannel_Wifi(); // pick a channel
  slave.encrypt = 0; // no encryption
  // check if the peer exists
  bool exists = esp_now_is_peer_exist(slave.peer_addr);
  if (exists) {
    // Slave already paired.
    Serial.println("Already Paired so deleting pairing");
    esp_now_del_peer(peer_addr);

  } 
  esp_err_t addStatus = esp_now_add_peer(peer);
  if (addStatus == ESP_OK) {
    // Pair success
    Serial.println("Pair success");
    return true;
  } else {
    Serial.println("Pair failed");
    return false;
  }
} 
// callback when data is sent
void _OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.print(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success to " : "Delivery Fail to ");
  _printMAC(mac_addr);
  Serial.println();
}

void _OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  int _size = getListMesures()->length;
  unsigned long _nextIdMesure=getNextIdMesure();
  board * _board=NULL;
  uint8_t type = incomingData[0];       // first message byte is the type of message 
  switch (type) {
  case DATA :                           // the message is data type
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    _board=getBoardById(incomingReadings.id);
      Serial.println(incomingReadings.seqNum);
    if(_board!=NULL) {
      for(int i=0; i<getNumberOfValues(_board->mesureTypes);i++) {
        StaticJsonDocument<1000> carte;
        String payload;
        mesure *_mesure=creerMesure(_board,incomingReadings.seqNum, incomingReadings.valeurs[i],incomingReadings.stringValues[i],i);
        ajouterMesure(_mesure);
        carte["id"] = _mesure->boardId;
        carte["type"] = String(_mesure->typeId);
        carte["value"] = String(_mesure->value,2);
        carte["stringValue"] = String(_mesure->stringValue);
        carte["time_stamp"] = _mesure->timeStamp;
        carte["owner"] = getBoardById(_mesure->boardId)->owner;
        carte["SDstatus"] = getSDstatus_File()?"SD en place":"SD manquante";
        carte["nextMesureId"] = _nextIdMesure;
        carte["nbMesures"] = _size;
        carte["wifi"] = getBoxSsid_Wifi() + " : " + String(isConnected_Wifi()?"connecte":"non connecte");
        if(getBoxSsid_Wifi().length()==0) carte["wifi"] = "Aucun acces demande";
        serializeJson(carte, payload);
        Serial.print("event send :");
        serializeJson(carte, Serial);
        // ATTENTION A REMETTRE 
        //events.send(payload.c_str(), "new_readings", millis());
        sendWebEvents(payload);
        _board->payloads[i]=String(payload.c_str());
        //libérer l'espace payloads[i] ?
        //Serial.println(mesureToString(_mesure));
      }
    } else {
       Serial.println("La carte "+ String(incomingReadings.id) + "n'a pas été déclaréee dans ce programme");
    }
    break;
  
  case PAIRING:                            // the message is a pairing request 
    memcpy(&pairingData, incomingData, sizeof(pairingData));
    Serial.print("PAIRING request from: ");
    _printMAC(mac_addr);Serial.print(" (id : ");Serial.print(pairingData.id); Serial.print(") ");
    Serial.print(" on channel: "); Serial.print(pairingData.channel); Serial.print(" Server channel is : "); Serial.println(getChannel_Wifi());
    /*if (pairingData.id > 0 && pairingData.channel==chan) {     // do not replay to server itself*/
    if (pairingData.id > 0 ) {     // do not replay to server itself
      pairingData.id = 0;       // 0 is server for the response
      // Server is in AP_STA mode: peers need to send data to server soft AP MAC address 
      WiFi.softAPmacAddress(pairingData.macAddr);
      //WiFi.softAPmacAddress(mac_addr);   
      pairingData.channel=getChannel_Wifi();
      Serial.println("send response to the pairing request");
      esp_err_t result = esp_now_send(mac_addr, (uint8_t *) &pairingData, sizeof(pairingData));
      _addPeer(mac_addr);
    }  
    break; 
  }
}

bool setup_ESP_NOW(){
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      return false;
    }
    esp_now_register_send_cb(_OnDataSent);
    esp_now_register_recv_cb(_OnDataRecv);
    return true;
} 

/*void setChan_ESP_NOW(int c){
  chan = c;
}*/
