#ifndef sm_wifi
#define sm_wifi

#include <WiFi.h>
#include <Arduino.h>
#include "TimeLib.h"

#include "sm_file.h"

bool setup_Wifi();
bool isConnected_Wifi();
String getBoxSsid_Wifi();
int getAlarmLedPin_Wifi();
int getChannel_Wifi();
bool shouldBeConnected_Wifi();
// interface pour la date et l'heure
unsigned long get_EpochTime_Wifi();
void set_EpochTime_Wifi(unsigned long ep);
unsigned long getCurrentTime_Wifi();
String timeToString_Wifi(unsigned long _t);

#endif
