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

#include "ESPMgr.h"


/*** GLOBAL VARIABLES ***/

ESPMgr mgr;


/*** SETUP ***/

void setup() {
  mgr.begin();
  mgr.loadConfig();
  mgr.connectWifi();

  // DEBUG
  Serial.println();
  ESPTime ntp;
  ntp.getNTPepoch(mgr.readParamValue("time_server"));
  ntp.print();
  ESPTime saved = mgr.now();
  saved.print();
  Serial.println(saved.delta(&ntp));
  Serial.println();

  // debug
  pinMode(D6, OUTPUT);

  char message[100] = "Hello from ESP8266 ";
  mgr.mqttPublish(message);

  mgr.halt();
}


/*** EMPTY LOOP ***/

void loop() {

  // DEBUG
  bool state = HIGH;
  while (true) {
    digitalWrite(D6, state);
    delay(1);
  }
}
