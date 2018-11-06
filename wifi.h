/*
 * wifi.h
 */

#ifndef WIFI_H


/*** DEFINES ***/

#define WIFI_H

#define TYPE_IP     0
#define TYPE_BSSID  1

#define CONNECTED   0

/*** FUNCTIONS ***/

unsigned char* charToAddr(char* address, int type);
int charToInt(char* number, char* format);
void printAddr(unsigned char* addr, int type);
int connectSaved();
int connectClean();


#endif

