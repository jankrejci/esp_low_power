/*
 * mqtt.h
 */


#ifndef MQTT_H

/*** INCLUDES ***/

#include "PubSubClient.h"
#include "wifi.h"


/*** DEFINES ***/

#define MQTT_H

#define MQTT_CONNECTED  0
#define MQTT_FAILED     1


/*** FUNCTIONS ***/
int mqttConnect();
void mqttPublish(char* message);

#endif

