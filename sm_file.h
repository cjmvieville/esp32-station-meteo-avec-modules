#ifndef sm_file
#define sm_file

#include <Arduino.h>

#include "FS.h"
#include "SD_MMC.h" 
#include "SPI.h"
#include "sm_wifi.h"

bool setup_File();
void print_File(char * path);
String readHtml_File(char * filename);
bool testSDCard_File();
bool getSDstatus_File();
void append_File(const char * message) ;
void writeLog_File(String message);
#endif
