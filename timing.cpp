/*
 * time.cpp
 */

/*** INCLUDES ***/

#include "timing.h"
#include "config.h"

// TODO ingludes and globals in the header file
#include "WiFiUdp.h"
#include "DNSServer.h"



/*** GLOBAL VARIABLES ***/

uint32_t sync_seconds = 0;
uint32_t sync_millis = 0;

byte packetBuffer[NTP_PACKET_SIZE];
WiFiUDP udp;


/*** FUNCTIONS ***/

/*
 * 
 */
uint32_t getNTPepoch(bool sync) {
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "NTP sending request"); }

  udp.begin(UDP_LOCAL_PORT);  
  IPAddress timeServerIP;
  WiFi.hostByName(readValue("ntp_server"), timeServerIP);  //get a random server from the pool
  sendNTPpacket(timeServerIP);  // send an NTP packet to a time server

  uint16_t request_start = millis();
  
  delay(200);  // wait to see if a reply is available
  int cb = udp.parsePacket();
  if (!cb) {
    if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "NTP no packet received"); }
    return 0;
  } else {
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    uint32_t ntp_seconds = (uint32_t)packetBuffer[40] << 24
                         | (uint32_t)packetBuffer[41] << 16
                         | (uint32_t)packetBuffer[42] <<  8
                         | (uint32_t)packetBuffer[43] <<  0;

    uint32_t ntp_millis = (uint32_t)packetBuffer[44] << 24
                        | (uint32_t)packetBuffer[45] << 16
                        | (uint32_t)packetBuffer[46] << 8
                        | (uint32_t)packetBuffer[47] << 0;
    
    ntp_seconds -= 2208988800UL;
    ntp_millis = ((uint64_t) ntp_millis * 1000) >> 32;

    uint16_t request_time = millis() - request_start;

    if(sync) {      
      sync_seconds = ntp_seconds;
      sync_millis = ntp_millis;

      uint16_t delta = millis();
      if (sync_millis > delta){
        sync_millis -= delta;
      } else {
        sync_seconds -= 1;
        sync_millis = 1000 + sync_millis - delta;
      }
    } 

    // TODO debug
    int32_t delta = (int32_t)(nowSeconds() - ntp_seconds) * 1000 + nowMillis() - ntp_millis + request_time;
    Serial.printf("%04d: %s: %ld\n", millis(), "TIME DELTA", delta);
    
    return(ntp_seconds + request_time / 1000);
  }  
}


/*
 * send an NTP request to the time server at the given address
 */
// helper function to build NTP packet
void sendNTPpacket(IPAddress& address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  udp.beginPacket(address, NTP_PORT); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}


/*
 * 
 */
void syncTimeNTP(){
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Syncing time from NTP"); }
  getNTPepoch(true);  
}


/*
 * 
 */
void syncTimeSaved(){
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Syncing time from saved param"); }
  
  uint32_t saved_seconds;
  sscanf(readValue("time_wake_s"), "%lu", &saved_seconds);

  uint32_t saved_millis;
  sscanf(readValue("time_wake_ms"), "%lu", &saved_millis);

  uint16_t delta = millis();
  sync_millis = (saved_millis + delta) % 1000;
  sync_seconds = saved_seconds + (saved_millis + delta) / 1000;
}


/*
 * writes expected wake up time
 */
void writeTimeParam(){  
  char buf[VALUE_LEN];
  int16_t delta = 300;
  
  uint32_t expected_seconds = nowSeconds() + (uint32_t)(SLEEP_TIME / 1e6) - 1;
  int32_t expected_millis = nowMillis() + delta;

  if(expected_millis > 1000){
    expected_seconds += 1;
    expected_millis %= 1000;
  }
  
  sprintf (buf, "%lu", expected_seconds);
  writeValue("time_wake_s", buf);
  
  sprintf (buf, "%ld", expected_millis);
  writeValue("time_wake_ms", buf);
  
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Saving time"); }
  if(VERBOSITY) { Serial.printf("%04d: %s %lu\n", millis(), "Saved seconds", expected_seconds); }
  if(VERBOSITY) { Serial.printf("%04d: %s %lu\n", millis(), "Saved  millis", expected_millis); }
}



/*
 * 
 */
uint32_t getEpoch(){
  return(nowSeconds());
}


/*
 * 
 */
uint32_t getLastUpdateDelta() {
  uint32_t last_update_time;
  sscanf(readValue("wifi_time"), "%lu", &last_update_time);
  return(nowSeconds() - last_update_time);
}


/*
 * 
 */
void writeWifiTimeParam() {
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Writing WiFi update time"); }
  char buf[VALUE_LEN];
  sprintf (buf, "%lu", nowSeconds());
  writeValue("wifi_time", buf);
}


/*
 * 
 */
uint32_t nowSeconds(){
  uint32_t actual_millis = sync_millis + millis();
  return(sync_seconds + actual_millis / 1000); 
}


/*
 * 
 */
uint16_t nowMillis(){
  uint32_t actual_millis = sync_millis + millis();
  return((uint16_t)actual_millis % 1000); 
}
