/*
 * time.h
 */

#ifndef TIMING_H


/*** INCLUDES ***/

#include "wifi.h"


/*** DEFINES ***/

#define TIMING_H

#define NTP_PORT          123
#define NTP_PACKET_SIZE   48

/*** GLOBAL VARIABLES ***/


/*** FUNCTIONS ***/
uint32_t getNTPepoch(bool sync);
void sendNTPpacket(IPAddress& address);
void syncTimeNTP();
void syncTimeSaved();
void writeTimeParam();
uint32_t getEpoch();
uint32_t getLastUpdateDelta();
void writeWifiTimeParam();
uint32_t nowSeconds();
uint16_t nowMillis();


#endif
