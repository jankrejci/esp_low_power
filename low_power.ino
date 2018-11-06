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
#include <ESP8266WiFi.h>
#include "config.h"


/*** DEFINES ***/

// verbosity of the debug output: 0 - no msg, 1 - status msg only, 2 - all msg
#define VERBOSITY    2


/*** GLOBAL VARIABLES ***/


/*** SETUP ***/

void setup() {
  // do not start wifi during the boot process
  // WiFi.mode(WIFI_OFF);
  // WiFi.forceSleepBegin();
  delay(1);

  // initialize serial communication for debug
  if (VERBOSITY) {
    Serial.begin(115200);
    // TODO check the status
    delay(1);
    Serial.println();
    Serial.println("=== ESP started ===");
  }

  SPIFFS.begin();
  // TODO check te status
  if(VERBOSITY) { Serial.println("Filesystem started"); }

  //reset saved settings if jumper is mounted
  pinMode(D5, INPUT_PULLUP);
  if(!digitalRead(D5)) {
    if(VERBOSITY) { Serial.println("Configuration reset"); }
    // TODO start config server only
    SPIFFS.format();
  }

  saveParams(CONFIG_FILE);

  if(VERBOSITY >=2) { printFile(CONFIG_FILE); }

  // TODO handle wrong loading
  switch(loadParams(CONFIG_FILE)){
    case CONFIG_OK:
      break;
    case FS_ERROR:
      break;
    case NO_CONFIG_FILE:
      break;
    case CONFIG_READ_FAIL:
      break;
    default:
      break;
  }
  if(VERBOSITY >= 2) { printParams(); }

  if(strcmp(getValue("wifi_ssid"), "") == 0){
    Serial.println("No WiFi SSID available");
    // TODO handle no SSID
  }

  Serial.print("IP: #");
  Serial.print(strcmp(getValue("wifi_ip"), ""));
  Serial.println("#");
  if(strcmp(getValue("wifi_ip"), "") == 0){
    Serial.println("Connecting to wifi");
    WiFi.forceSleepWake();
    delay( 1 );
    WiFi.mode(WIFI_STA);
    WiFi.begin(getValue("wifi_ssid"), getValue("wifi_pass"));
    delay(1000);
    Serial.println(WiFi.localIP());
  }


}


/*** LOOP ***/

void loop() {}
