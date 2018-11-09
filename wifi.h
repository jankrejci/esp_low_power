/*
 * wifi.h
 */

#ifndef WIFI_H

/*** INCLUDES ***/

#include "ESP8266WiFi.h"
#include "config.h"


/*** DEFINES ***/

#define WIFI_H

#define TYPE_IP               0
#define TYPE_BSSID            1

#define WIFI_CONNECTED        0
#define WIFI_CONNECTION_FAIL  1

#define WIFI_DELAY            100 // time in ms, how often to check wifi status while connecting
#define WIFI_STAT_MSG         3   // time in s, how often to print status msg while connecting
#define WIFI_TIMEOUT          10  // time in s, how long to try to connect to wifi

#define RST_DEEP_SLEEP        5

#define UDP_LOCAL_PORT        2390


/*** GLOBAL VARIABLES ***/




/*** FUNCTIONS ***/

unsigned char* charToAddr(char* address, int type);
int charToInt(char* number, char* format);
void printAddr(unsigned char* addr, int type);
int connectSaved();
int connectClean();
void writeWifiParams();
void haltESP();
void haltESP(unsigned long sleep);
void initWiFi();
int connectionCheck();
int espResetInfo();
void resetWifiParams();
//void initUDP();


#endif

