/*
 * wifi.cpp
 */


/*** INCLUDES ***/


#include <ESP8266WiFi.h>
#include "wifi.h"
#include "config.h"

 /*
 * 
 */
unsigned char* charToAddr(char* address, int type){
  int count = 4;
  char delim = '.';
  char* format = "%d";
    
  if (type == TYPE_BSSID) {
    count = 6;
    delim = ':';
    format = "%x";
  }
  
  char temp[20];
  strcpy(temp, address);
  
  char* last_ptr = &temp[0];
  unsigned char addr[count];
  int index = 0;
  
  for (int i = 0; i < 20; i++){
    if(index == count){
      break;
    }
    if(temp[i] == 0){      
      addr[index] = charToInt(last_ptr, format);
      break;
    }
    if(temp[i] == delim){
      temp[i] = 0;
      addr[index] = charToInt(last_ptr, format);
      last_ptr = &temp[i+1];
      index++;
    }
  }
  return(addr);
}


/*
 * 
 */
int charToInt(char* number, char* format){
  int value;
  sscanf(number, format, &value);
  return(value);
}


/*
 * 
 */
void printAddr(unsigned char* addr, int type){
  int count = 4;
  char delim = '.';
  char* format = "%d";
 
  if (type == TYPE_BSSID) {
    count = 6;
    delim = ':';
    format = "%x";
  }

  for(int i = 0; i < count; i++){
    Serial.printf(format, addr[i]);
    if(i < count - 1){
      Serial.printf("%s", delim);
    }
  }
  Serial.printf("\n");
}


/*
 * 
 */
int connectSaved(){
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Connecting to wifi with last setting"); }
  WiFi.forceSleepWake();
  delay( 1 );
  
  WiFi.mode(WIFI_STA);
  WiFi.config(charToAddr(readValue("wifi_ip"),    TYPE_IP),
              charToAddr(readValue("wifi_dns"),   TYPE_IP),
              charToAddr(readValue("wifi_gate"),  TYPE_IP),
              charToAddr(readValue("wifi_mask"),  TYPE_IP));
              
  int wifi_chan;
  sscanf(readValue("wifi_chan"), "%d", &wifi_chan);
  
  WiFi.begin( readValue("wifi_ssid"),
              readValue("wifi_pass"),
              wifi_chan,
              charToAddr(readValue("wifi_bssid"), TYPE_BSSID),
              true);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  if (VERBOSITY) { Serial.printf("%04d: %s: %s\n", millis(), "Connected to WiFI with IP", WiFi.localIP().toString().c_str()); }
  return(CONNECTED);
}


/*
 * 
 */
int connectClean(){
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Connecting to wifi with DHCP"); }
  WiFi.forceSleepWake();
  delay( 1 );
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(readValue("wifi_ssid"), readValue("wifi_pass"));
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  if (VERBOSITY) { Serial.printf("%04d: %s: %s\n", millis(), "Connected to WiFI with IP", WiFi.localIP().toString().c_str()); }

  writeValue("wifi_ip", WiFi.localIP().toString().c_str());
  writeValue("wifi_mask", WiFi.subnetMask().toString().c_str());
  writeValue("wifi_gate", WiFi.gatewayIP().toString().c_str());
  writeValue("wifi_dns", WiFi.dnsIP().toString().c_str());
  
  uint8_t* temp = WiFi.BSSID();
  char bssid[20];
  for(int i = 0; i<6; i++){      
    char buf[4];
    sprintf (buf, "%02X", (int)temp[i]);
    strcat(bssid, buf);
    if(i < 5){
      strcat(bssid, ":");
    }
  }
  writeValue("wifi_bssid", bssid);

  char buf[4];
  sprintf (buf, "%02i", WiFi.channel());
  writeValue("wifi_chan", buf);

  if (VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Connection parameters has been written"); }
}

