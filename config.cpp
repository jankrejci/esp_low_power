/*
 * config.cpp
 */

/*** INCLUDES ***/

#include "FS.h"
#include "config.h"


/*** GLOBAL VARIABLES ***/

struct Param params[] = {
                          // param name, nice name, default value
                          {"wifi_ssid",   "SSID",       "WiFi VNT"},
                          {"wifi_pass",   "Password",   "vnttnv321"},
                          {"wifi_chann",  "Channel",    ""},
                          {"wifi_essid",  "ESSID",      ""},
                          {"wifi_ip",     "IP",         ""},
                          {"wifi_gate",   "Gateway",    ""},
                          {"wifi_mask",   "Mask",       ""},
                          {"mqtt_server", "Server",     "skynjari.cz"},
                          {"mqtt_port",   "Port",       "1883"},
                          {"mqtt_user",   "User name",  ""},
                          {"mqtt_pass",   "Password",   ""},
                          {"mqtt_id",     "ID",         "esp_sleep"},
                          {"mqtt_topic",  "Topic",      "esp/test"},
                          {"timezone",    "Timezone",   "2"}
                        };

#define params_count sizeof(params) / sizeof(params[0])


/*** FUNCTIONS ***/

/*
 * return value by key
 */
char* getValue(const char* key) {
  struct Param param = readParam(key);
  if(strcmp(param.key, "") == 0){
    return("");
  }
  return (param.value);
}


/*
 * return param by key
 */
struct Param readParam(const char* key) {
  for(int i = 0; i < params_count; i++){
    if(strcmp(params[i].key, key) == 0){
      return(params[i]);
    }
  }
  return(Param{});
}


/*
 * write values to param
 */
int writeParam(char* key, char* nice_name, char* value) {
  for(int i = 0; i < params_count; i++){
    if(strcmp(params[i].key, key) == 0){
      strcpy(params[i].nice_name, nice_name);
      strcpy(params[i].value, value);
      return(0);
    }
  }
  return(1);
}


/*
 *
 */
void saveParams(char* filename) {
  // TODO VERBOSITY
  File file = SPIFFS.open(filename, "w");
  if (!file) {
      Serial.println("File open failed");
  } else {
    Serial.print("Writing configuration to: ");
    Serial.println(filename);
    // write 10 strings to file
    for (int i = 0; i < params_count; i++){
      file.print(params[i].key);
      file.print(DELIMITER);
      file.print(params[i].nice_name);
      file.print(DELIMITER);
      file.println(params[i].value);
    }
  }
  file.close();
}


/*
 *
 */
int loadParams(char* filename) {
  // TODO VERBOSITY
  File file = SPIFFS.open(filename, "r");
  if (!file) {
      Serial.println("File open failed");
  } else {
    Serial.print("Loading config file: ");
    Serial.println(filename);
    char line_buffer[LINE_BUFFER];
    while (file.available()){
      int len = file.readBytesUntil('\n', line_buffer, LINE_BUFFER);
      line_buffer[len] = 0;

      char** splitted = splitLine(line_buffer);
      writeParam(splitted[KEY], splitted[NICE_NAME], splitted[VALUE]);
      // TODO check the result of write
    }
  }
  file.close();
}


/*
 * prints content of the file
 */
void printFile(char* filename) {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    Serial.println("File open failed");
  } else {
    Serial.println();
    Serial.print("Reading file: ");
    Serial.println(filename);
    while (file.available()){
      Serial.write(file.read());
    }
  }
  Serial.println();
  file.close();
}


/*
 *
 */
void printParams() {
  Serial.println();
  Serial.println("Configuration parameters:");
  for(int i = 0; i < params_count; i++){
    Serial.print(params[i].key);
    Serial.print(": [");
    Serial.print(params[i].nice_name);
    Serial.print(", ");
    Serial.print(params[i].value);
    Serial.println("]");
  }
  Serial.println();
}


/*
 * splits line to three char arrays
 * returns pointers to begining of strings
 */
char** splitLine(char* line){
  char* pointers[3];
  Serial1.println((int)&line);  // workaround
  pointers[0] = &line[0];
  int p_index = 1;

  for (int i = 0; i < LINE_BUFFER; i++){
    if (line[i] == DELIMITER) {
      pointers[p_index] = &line[i+1];
      p_index++;
      line[i] = 0;
      i++;
    }
    if (p_index > 2){
      break;
    }
  }
  return(pointers);
}
