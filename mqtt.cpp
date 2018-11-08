/*
 * mqtt.cpp
 */


/*** INCLUDES ***/

#include "mqtt.h"


/*** GLOBAL VARIABLES ***/

WiFiClient espClient;
PubSubClient mqttClient(espClient);


/*** FUNCTIONS ***/

/*
 * 
 */
int mqttConnect(){
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Connecting to MQTT"); }  
  int port;
  sscanf(readValue("mqtt_port"), "%d", &port);  
  mqttClient.setServer(readValue("mqtt_server"), port);
  if (!mqttClient.connect(readValue("mqtt_id"), readValue("mqtt_user"), readValue("mqtt_pass"))) {
    if(VERBOSITY) { Serial.printf("%04d: %s: %d\n", millis(), "MQTT failed with state", mqttClient.state()); }
    return(MQTT_FAILED);
  }
  if(VERBOSITY) { Serial.printf("%04d: %s %s:%d\n", millis(), "MQTT connected to", readValue("mqtt_server"), port); }
  return(MQTT_CONNECTED);
}


/*
 * 
 */
void mqttPublish(char* message){
  mqttClient.publish(readValue("mqtt_topic"), message);
  if(VERBOSITY) { Serial.printf("%04d: %s: %s, %s, %s\n", millis(), "MQTT Pub", readValue("mqtt_id"), readValue("mqtt_topic"), message); }
  delay(10);
  mqttClient.disconnect();
}

