/*
 * LINK NTP time https://www.geekstips.com/arduino-time-sync-ntp-server-esp8266-udp/
 * LINK NTP time https://tttapa.github.io/ESP8266/Chap15%20-%20NTP.html
 * LINK time lib https://www.pjrc.com/teensy/td_libs_Time.html
 * LINK I2C wiring https://www.14core.com/wiring-nodemcu-esp8266-12e-with-i2c-16x2-lcd-screen/
 * LINK simple IoT https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/
 * LINK python client https://www.cloudmqtt.com/docs-python.html
 * LINK IoT publish https://techtutorialsx.com/2017/04/24/esp32-publishing-messages-to-mqtt-topic/
 * LINK deep sleep https://www.losant.com/blog/making-the-esp8266-low-powered-with-deep-sleep
 * LINK power save https://www.bakke.online/index.php/2017/05/21/reducing-wifi-power-consumption-on-esp8266-part-1/
 * LINK pro tips https://www.instructables.com/id/ESP8266-Pro-Tips/
 *
 */


/*** INCLUDES ***/

#include "FS.h"
#include "ESP8266WiFi.h"

#include "config.h"
#include "wifi.h"


/*** DEFINES ***/


/*** GLOBAL VARIABLES ***/


/*** SETUP ***/

void setup() {
  // do not start wifi during the boot process
  WiFi.mode(WIFI_OFF);
  WiFi.persistent(false);
  WiFi.forceSleepBegin();

  // initialize serial communication for debug
  if (VERBOSITY) {
    Serial.begin(115200);
    while(!Serial){
      delay(10);
    }
    Serial.printf("\n=== %s ===\n", "ESP started");
  }

  SPIFFS.begin();
  // TODO check te status
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Filesystem started"); }  
  //reset saved settings if jumper is mounted
  pinMode(D5, INPUT_PULLUP);
  if(!digitalRead(D5)) {
    if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Configuration reset"); }
    // TODO start config server only
    SPIFFS.format();
  }  

  // TODO handle wrong loading
  if(VERBOSITY >=2) { printFile(CONFIG_FILE); }
  switch(loadParams(CONFIG_FILE)){
    
    case NO_CONFIG_FILE:
      saveParams(CONFIG_FILE);
      Serial.printf("%04d: %s\n", millis(), "Going down"); 
      if(VERBOSITY) { Serial.printf("=== %s ===\n", "ESP Restart"); }
      ESP.deepSleep(1e6, WAKE_RF_DISABLED);
      delay(5000);
      
    default:
      break;
  }
  if(VERBOSITY >= 2) { printParams(); }

  // connect with saved parameters
  if(strlen(readValue("wifi_ip")) > 7){
    connectSaved();
    // TODO debug
    rmConfig(); 
  }

  // get the connection from DHCP
  if(strlen(readValue("wifi_ip")) == 0){
    connectClean();
    saveParams(CONFIG_FILE);
  }

  if(strlen(readValue("wifi_ssid")) == 0){
    Serial.printf("%04d: %s\n", millis(), "No WiFi SSID available");
    // TODO handle no SSID
  }
  
  if(VERBOSITY) {
    Serial.printf("%04d: %s\n", millis(), "Going down");
    Serial.printf("=== %s ===\n", "ESP Sleep"); 
  }

  
  ESP.deepSleep(10e6, WAKE_RF_DISABLED);
  delay(5000);
}


/*** LOOP ***/

void loop() {}


/*** FUNCTIONS ***/

