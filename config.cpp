/*
 * config.cpp
 */

/*** INCLUDES ***/

#include "config.h"


/*** GLOBAL VARIABLES ***/

struct Param params[] = {
  // param name, nice name, default value
  {"wifi_ssid",   "SSID",       "WiFi VNT"},
  {"wifi_pass",   "Password",   "vnttnv321"},
  {"wifi_chan",   "Channel",    ""},
  {"wifi_bssid",  "BSSID",      ""},
  {"wifi_ip",     "IP",         ""},                          
  {"wifi_mask",   "Mask",       ""},
  {"wifi_gate",   "Gateway",    ""},
  {"wifi_dns",    "DNS",        "8.8.8.8"},
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
char* readValue(const char* key) {
  char *value_ptr;
  for(int i = 0; i < params_count; i++){
    if(strcmp(params[i].key, key) == 0){
      value_ptr = params[i].value;
      return(value_ptr);
    }
  }
}

/*
 *
 */
void writeValue(const char* key, const char* value) {
  for(int i = 0; i < params_count; i++){
    if(strcmp(params[i].key, key) == 0){
      strcpy(params[i].value, value);
      break;
    }
  }
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
  File file = SPIFFS.open(filename, "w");
  if (!file) {
      if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(),"File open failed"); }
  } else {
    if(VERBOSITY) { Serial.printf("%04d: %s: %s\n", millis(), "Writing configuration to", filename); }
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
  File file = SPIFFS.open(filename, "r");
  if (!file) {
      if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(),"No configuration file found"); }
      return NO_CONFIG_FILE;
  } else {
    if(VERBOSITY) { Serial.printf("%04d: %s: %s\n", millis(),"Loading config file", filename); }
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
  return CONFIG_LOADED;
}


/*
 * prints content of the file
 */
void printFile(char* filename) {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "No file to print"); }
    return;
  } else {
    if(VERBOSITY) { Serial.printf("\n%04d: %s: %s\n", millis(), "Reading file: ", filename); }
    while (file.available()){
      Serial.write(file.read());
    }
  }
  Serial.printf("\n");
  file.close();
}


/*
 *
 */
void printParams() {
  if(VERBOSITY) {
    Serial.printf("\n%04d: %s\n", millis(), "Configuration parameters:");
    for(int i = 0; i < params_count; i++){
      Serial.printf("%s: [%s, %s]\n", params[i].key, params[i].nice_name, params[i].value);
    }
    Serial.printf("\n");
  }
}


/*
 * 
 */
void printString(char* text){
  if(VERBOSITY){
    Serial.printf("\n%04d: %s: %x\n", millis(), "Addr: ", (int)text);
    for(int i = 0; i < LINE_BUFFER; i++){
      Serial.printf("%02d:%s:%d", i, text[i], (int)text[i]);
      if((int)text[i] == 0){
        break;
      }
    }
  }
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

  // clean non printable characters
  for (int i = 0; i < LINE_BUFFER; i++){
    if((int)line[i] == 0){
      break;
    }
    if((int)line[i] < 32){
      line[i] = 0;
    }
  }

  // split to three strings
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


/*
 * 
 */
void rmConfig() {
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Deleting config file"); }
  SPIFFS.remove(CONFIG_FILE);
}


/*
 * initialize serial communication for debug
 */
void initSerial(){
  if (VERBOSITY) {
    Serial.begin(SERIAL_SPEED);
    while(!Serial){
      delay(10);
    }
    Serial.printf("\n=== %s ===\n", "ESP started");
  }
}


/*
 * initialize the filesystem
 */
void initFS(){
  SPIFFS.begin();  // TODO check the status  
  if(VERBOSITY) { Serial.printf("%04d: %s\n", millis(), "Filesystem started"); }
}

