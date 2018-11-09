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

#include "config.h"
#include "wifi.h"
#include "mqtt.h"
#include "timing.h"


/*** DEFINES ***/


/*** GLOBAL VARIABLES ***/


/*** SETUP ***/

void setup() {
  initWiFi();     // sleep mode right after the start
  initSerial();   // communication line for debug info
  initFS();       // file system for config file
  
  //reset saved settings if jumper is mounted
  pinMode(D5, INPUT_PULLUP);
  if(!digitalRead(D5)) {
    if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Configuration reset"); }
    rmConfig();  // TODO start config server only
  }  

  // TODO handle wrong loading
  switch(loadParams(CONFIG_FILE)){
    case CONFIG_LOADED:
      if(VERBOSITY >=2) { printFile(CONFIG_FILE); }
      break;
          
    case NO_CONFIG_FILE:
      saveParams(CONFIG_FILE);  // create config file with default params
      haltESP(1e6);             // restart
      break;

    default:
      break;
  }
  if(VERBOSITY >= 2) { printParams(); }

  // clean wifi and time setting if there was power loss
  int info = espResetInfo();
  if(VERBOSITY >= 2) { Serial.printf("%04d: %s: %d\n", millis(), "Reset status", info); }
  if(info != RST_DEEP_SLEEP){
    resetWifiParams();
  }
  
  // connect with saved parameters
  if(strlen(readValue("wifi_ip")) > 7){
    connectSaved();
    syncTimeSaved();
  }

  // get the connection from DHCP
  if(strlen(readValue("wifi_ip")) == 0 ||
     strcmp(readValue("wifi_ip"), "0.0.0.0") == 0 ||
     getLastUpdateDelta() > 3600){
    connectClean();
    writeWifiParams();
    syncTimeNTP();
    writeWifiTimeParam();
  }

  // TODO handle no config, no connection
  if(strlen(readValue("wifi_ssid")) == 0){
    if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "No WiFi SSID available"); }
  }

  uint32_t ntp_sec = getNTPepoch(false);
  if(VERBOSITY) { Serial.printf("%04d: %s: %lu\n", millis(), "Actual time", ntp_sec); }
  if(VERBOSITY) { Serial.printf("%04d: %s: %lu\n", millis(), "Expect time", nowSeconds()); }
  
  
  char message[100] = "Hello from ESP8266 ";
  if(mqttConnect() == MQTT_CONNECTED) {
    mqttPublish(message);
  }

  writeTimeParam();
  saveParams(CONFIG_FILE);
  haltESP();
}


/*** LOOP ***/

void loop() {}


/*** FUNCTIONS ***/
