/*
 * ESPMgr.cpp
 */


/*** INCLUDES ***/

#include "ESPMgr.h"


/*** FUNCTIONS ***/

/*
 *
 */
ESPMgr::ESPMgr() {

}


/*
 *
 */
ESPMgr::~ESPMgr() {

}


/*
 *
 */
void ESPMgr::begin(){
  // DEBUG
  pinMode(D8, OUTPUT);
  digitalWrite(D8, HIGH);

  // do not start wifi during the boot process
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

  // get reset info to recognize power down
  rst_info *rsti;
  rsti = ESP.getResetInfoPtr();
  _reset_info = rsti->reason;

  // initialize serial communication for debug
  if (_verbosity) {
    Serial.begin(SERIAL_SPEED);
    while(!Serial){
      delay(10);
    }
    Serial.printf("\n\n=== %s ===\n", "ESP started");
  }

  // initialize file system
  SPIFFS.begin();

  // delete configuration file if required
  pinMode(RESET_PIN, INPUT_PULLUP);
  if(!digitalRead(RESET_PIN)) {
    if(_verbosity) { Serial.printf( "%04d: %s\n",
                                    millis(),
                                    "Configuration reset"); }
    rmConfig();  // TODO start config server only
  }

  if(_verbosity) { Serial.printf( "%04d: %s\n",
                                  millis(),
                                  "Initialization done"); }
}


/*
 * return value by key
 */
char* ESPMgr::readParamValue(const char *key) {
  char *value_ptr;
  for(int i = 0; i < PARAMS_MAX; i++){
    if(strlen(_params[i].key) == 0){
      break;
    }
    if(strcmp(_params[i].key, key) == 0){
      value_ptr = _params[i].value;
      return(value_ptr);
    }
  }
}


/*
 *
 */
void ESPMgr::setVerbosity(uint8_t verbosity) {
  _verbosity = verbosity;
}


/*
 *
 */
void ESPMgr::rmConfig() {
  if(_verbosity) { Serial.printf( "%04d: %s \n",
                                  millis(),
                                  "Deleting config file"); }
  SPIFFS.remove(CONFIG_FILE);
}


/*
 *
 */
void ESPMgr::loadConfig() {
  switch(loadParams(CONFIG_FILE)){

    case CONFIG_LOADED:
      if(_verbosity >=2) { printFile(CONFIG_FILE); }
      break;

    case NO_CONFIG_FILE:
      saveParams(CONFIG_FILE);  // create config file with default params
      break;

    default:
      break;
  }
  if(_verbosity >= 2) { printParams(); }
  // TODO test
  syncTimeSaved();
}


/*
 *
 */
int ESPMgr::loadParams(char* filename) {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
      if(_verbosity) { Serial.printf( "%04d: %s\n",
                                      millis(),
                                      "No configuration file found"); }
      return NO_CONFIG_FILE;
  } else {
    if(_verbosity) { Serial.printf( "%04d: %s: %s\n",
                                    millis(),
                                    "Loading config file",
                                    filename); }
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
 *
 */
void ESPMgr::setSleepTime(uint32_t sleep_time){
  _sleep_time = sleep_time;
}


/*
 *
 */
void ESPMgr::printParams() {
    Serial.printf("\n%04d: %s\n",
                  millis(),
                  "Configuration parameters:");
    for(int i = 0; i < PARAMS_MAX; i++){
      if(strlen(_params[i].key) == 0){
        break;
      }
      Serial.printf("%s: [%s, %s]\n",
                    _params[i].key,
                    _params[i].nice_name,
                    _params[i].value);
    }
    Serial.printf("\n");
}


/*
 * splits line to three char arrays
 * returns pointers to begining of strings
 */
char** ESPMgr::splitLine(char* line){
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
void ESPMgr::saveParams(char* filename) {
  File file = SPIFFS.open(filename, "w");
  if (!file) {
      if(_verbosity) { Serial.printf( "%04d: %s\n",
                                      millis(),
                                      "File open failed"); }
  } else {
    if(_verbosity) { Serial.printf( "%04d: %s: %s\n",
                                    millis(),
                                    "Writing configuration to",
                                    filename); }
    // write 10 strings to file
    for (int i = 0; i < PARAMS_MAX; i++){
      if(strlen(_params[i].key) == 0){
        break;
      }
      file.print(_params[i].key);
      file.print(DELIMITER);
      file.print(_params[i].nice_name);
      file.print(DELIMITER);
      file.println(_params[i].value);
    }
  }
  file.close();
}


/*
 *
 */
void ESPMgr::writeValue(const char* key, const char* value) {
  for(int i = 0; i < PARAMS_MAX; i++){
    if(strlen(_params[i].key) == 0){
      break;
    }
    if(strcmp(_params[i].key, key) == 0){
      strcpy(_params[i].value, value);
      break;
    }
  }
}


/*
 * write values to param
 */
int ESPMgr::writeParam(char* key, char* nice_name, char* value) {
  for(int i = 0; i < PARAMS_MAX; i++){
    if(strlen(_params[i].key) == 0){
      break;
    }
    if(strcmp(_params[i].key, key) == 0){
      strcpy(_params[i].nice_name, nice_name);
      strcpy(_params[i].value, value);
      return(0);
    }
  }
  return(1);
}


/*
 * prints content of the file
 */
void ESPMgr::printFile(char* filename) {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    if(_verbosity) { Serial.printf( "%04d: %s\n",
                                    millis(),
                                    "No file to print"); }
    return;
  } else {
    if(_verbosity) { Serial.printf( "\n%04d: %s: %s\n",
                                    millis(),
                                    "Reading file: ",
                                    filename); }
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
void ESPMgr::halt(uint32_t sleep_time){
  // save config for next cycle
  writeTimeParam();
  saveParams(CONFIG_FILE);

  if(_verbosity) {
    Serial.printf("%04d: %s %d s\n",
                  millis(),
                  "Going down for",
                  (int)(sleep_time / 1e6));
    Serial.printf("=== %s ===\n", "ESP Sleep");
  }

  ESP.deepSleep(sleep_time);  // TODO WAKE_RF_DISABLED
  // DEBUG
  digitalWrite(D8, LOW);
  // delay(5000);
}


/*
 *
 */
void ESPMgr::halt(){
  halt(_sleep_time);
}


/*
 *
 */
void ESPMgr::connectWifi(){
  // reset wifi parameters if there was a power loss
  if(_verbosity) { Serial.printf( "%04d: %s: %d\n",
                                  millis(),
                                  "Reset status",
                                  _reset_info); }
  if(_reset_info != RST_DEEP_SLEEP){
    resetWifiParams();
  }

  // connect with saved parameters
  if(strlen(readParamValue("wifi_ip")) > 7){
    connectSaved();
  }

  // get the connection from DHCP
  if( strlen(readParamValue("wifi_ip")) == 0 ||
      strcmp(readParamValue("wifi_ip"), "0.0.0.0") == 0 ||
      getLastUpdateDelta() > 3600){
        connectDHCP();
        writeWifiParams();
        syncTimeNTP();
        writeWifiTimeParam();
  }

  // TODO handle no config, no connection
  if(strlen(readParamValue("wifi_ssid")) == 0){
      if(_verbosity) { Serial.printf( "%04d: %s\n",
                                      millis(),
                                      "No WiFi SSID available"); }
  }
}


/*
 *
 */
int ESPMgr::connectSaved(){
  if(_verbosity) { Serial.printf( "%04d: %s\n",
                                  millis(),
                                  "Connecting to wifi with last setting"); }
  WiFi.forceSleepWake();
  delay(10);

  WiFi.mode(WIFI_STA);
  WiFi.config(charToAddr(readParamValue("wifi_ip"),    TYPE_IP),
              charToAddr(readParamValue("wifi_dns"),   TYPE_IP),
              charToAddr(readParamValue("wifi_gate"),  TYPE_IP),
              charToAddr(readParamValue("wifi_mask"),  TYPE_IP));

  int wifi_chan;
  sscanf(readParamValue("wifi_chan"), "%d", &wifi_chan);

  WiFi.begin( readParamValue("wifi_ssid"),
              readParamValue("wifi_pass"),
              wifi_chan,
              charToAddr(readParamValue("wifi_bssid"), TYPE_BSSID),
              true);

  return(connectionCheck());
}


/*
 *
 */
int ESPMgr::connectDHCP(){
  if(_verbosity) { Serial.printf( "%04d: %s\n",
                                  millis(),
                                  "Connecting to wifi with DHCP"); }
  WiFi.forceSleepWake();
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(readParamValue("wifi_ssid"), readParamValue("wifi_pass"));
  return(connectionCheck());
}


/*
 *
 */
int ESPMgr::connectionCheck(){
  int counter = 0;
  int connection_time = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(WIFI_DELAY);
    counter++;
    connection_time = (int)(counter * WIFI_DELAY);
    if(connection_time % (WIFI_STAT_MSG * 1000) == 0) {
      if(_verbosity) { Serial.printf( "%04d: %s %d\n",
                                      millis(),
                                      "Wifi status",
                                      WiFi.status()); }
    }
    if(connection_time > WIFI_TIMEOUT * 1000){
      if(_verbosity) { Serial.printf( "%04d: %s %d\n",
                                      millis(),
                                      "Wifi connection fail - state",
                                      WiFi.status()); }
      return(WIFI_CONNECTION_FAIL);
    }
  }
  if (_verbosity) { Serial.printf("%04d: %s: %s\n",
                                  millis(),
                                  "Connected to WiFI with IP",
                                  WiFi.localIP().toString().c_str()); }
  return(WIFI_CONNECTED);
}


/*
 *
 */
unsigned char* ESPMgr::charToAddr(char* address, int type){
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
int ESPMgr::charToInt(char* number, char* format){
  int value;
  sscanf(number, format, &value);
  return(value);
}


/*
 *
 */
void ESPMgr::printAddr(unsigned char* addr, int type){
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
void ESPMgr::writeWifiParams(){
  if (_verbosity) { Serial.printf("%04d: %s\n",
                                  millis(),
                                  "Writing WiFi configuration"); }

  writeValue("wifi_ip",   WiFi.localIP().toString().c_str());
  writeValue("wifi_mask", WiFi.subnetMask().toString().c_str());
  writeValue("wifi_gate", WiFi.gatewayIP().toString().c_str());
  writeValue("wifi_dns",  WiFi.dnsIP().toString().c_str());

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

  if(_verbosity >= 2) { printParams(); }
}



/*
 *
 */
void ESPMgr::resetWifiParams() {
  if(_verbosity) { Serial.printf( "%04d: %s\n",
                                  millis(),
                                  "Reseting WiFi temp params"); }

  char* keys[] = {"wifi_chan",
                  "wifi_bssid",
                  "wifi_ip",
                  "wifi_mask",
                  "wifi_gate",
                  "wifi_dns"};

  int key_count = sizeof(keys) / sizeof(keys[0]);
  for(int i = 0; i < key_count; i++){
    writeValue(keys[i], "");
  }

  if(_verbosity >= 2) { printParams(); }
}


/*
 *
 */
void ESPMgr::syncTimeSaved(){
  if(_verbosity) { Serial.printf( "%04d: %s\n",
                                  millis(),
                                  "Syncing time from saved param"); }

  uint32_t saved_seconds;
  sscanf(readParamValue("time_wake_s"), "%lu", &saved_seconds);

  uint32_t saved_millis;
  sscanf(readParamValue("time_wake_ms"), "%lu", &saved_millis);

  this->_sync_time.setTime(saved_seconds, saved_millis);
  this->_sync_time.addMillis(-1 * millis());
}


/*
 *
 */
void ESPMgr::syncTimeNTP(){
  if(_verbosity) { Serial.printf( "%04d: %s\n",
                                  millis(),
                                  "Syncing time from NTP"); }

  this->_sync_time.getNTPepoch(this->readParamValue("time_server"));
  this->_sync_time.addMillis(-1 * millis());
}


/*
 *
 */
uint32_t ESPMgr::getLastUpdateDelta() {
  uint32_t last_update_seconds;
  sscanf(readParamValue("wifi_time"), "%lu", &last_update_seconds);

  ESPTime actual_time = this->now();
  return(actual_time.getSeconds() - last_update_seconds);
}


/*
 *
 */
void ESPMgr::writeWifiTimeParam() {
  if(_verbosity) { Serial.printf( "%04d: %s\n",
                                  millis(),
                                  "Writing WiFi update time"); }

  ESPTime actual_time;
  actual_time = this->now();

  char buf[VALUE_LEN_MAX];
  sprintf (buf, "%lu", actual_time.getSeconds());
  writeValue("wifi_time", buf);
}


/*
 * writes expected wake up time
 */
void ESPMgr::writeTimeParam(){
  char buf[VALUE_LEN_MAX];
  ESPTime expected_time;

  expected_time = this->now();
  expected_time.addMillis(_sleep_time / 1000);

  int16_t delta = 570;
  expected_time.addMillis(delta);

  sprintf(buf, "%lu", expected_time.getSeconds());
  writeValue("time_wake_s", buf);

  sprintf(buf, "%ld", expected_time.getMillis());
  writeValue("time_wake_ms", buf);

  if(_verbosity) { Serial.printf( "%04d: %s: %lu\.%lu\n",
                                  millis(),
                                  "Time saved",
                                  expected_time.getSeconds(),
                                  expected_time.getMillis()); }
}


/*
 *
 */
ESPTime ESPMgr::now(){
  ESPTime actual_time = this->_sync_time;
  actual_time.addMillis(millis());
  return(actual_time);
}


/*
 *
 */
int ESPMgr::mqttConnect(){
  if(_verbosity) { Serial.printf( "%04d: %s\n",
                                  millis(),
                                  "Connecting to MQTT"); }

  _mqttClient.setClient(_espClient);
  int port;
  sscanf(readParamValue("mqtt_port"), "%d", &port);
  _mqttClient.setServer(readParamValue("mqtt_server"), port);

  if (!_mqttClient.connect( readParamValue("mqtt_id"),
                            readParamValue("mqtt_user"),
                            readParamValue("mqtt_pass"))) {
    if(_verbosity) { Serial.printf( "%04d: %s: %d\n",
                                    millis(),
                                    "MQTT failed with state",
                                    _mqttClient.state()); }
    return(MQTT_FAILED);
  }
  if(_verbosity) { Serial.printf( "%04d: %s %s:%d\n",
                                  millis(),
                                  "MQTT connected to",
                                  readParamValue("mqtt_server"),
                                  port); }
  return(MQTT_CONNECTED);
}


/*
 *
 */
void ESPMgr::mqttPublish(char* message){
  if(mqttConnect() == MQTT_CONNECTED) {
    _mqttClient.publish(readParamValue("mqtt_topic"), message);
    if(_verbosity) { Serial.printf( "%04d: %s: %s, %s, %s\n",
                                    millis(),
                                    "MQTT Pub",
                                    readParamValue("mqtt_id"),
                                    readParamValue("mqtt_topic"),
                                    message); }
    delay(10);
    _mqttClient.disconnect();
  }
}


/*** TIME CLASS ***/

/*
 *
 */
ESPTime::ESPTime(){
  _seconds = 0;
  _millis = 0;
}


/*
 *
 */
ESPTime::~ESPTime(){

}


/*
 *
 */
void ESPTime::setTime(uint32_t seconds, uint32_t millis) {
  _seconds = seconds;
  _millis = millis;
}


/*
 *
 */
uint32_t ESPTime::getSeconds(){
  return(_seconds);
}


/*
 *
 */
uint32_t ESPTime::getMillis() {
  return(_millis);
}


/*
 *
 */
void ESPTime::addMillis(int32_t millis) {
  int32_t new_millis;
  int32_t delta_seconds;
  new_millis = (int32_t)(_millis + millis);

  delta_seconds = new_millis / 1000;
  new_millis = new_millis % 1000;

  if (new_millis < 0) {
    _seconds -= 1;
    _millis = 1000 + new_millis;
  } else {
    _millis = new_millis;
  }

  _seconds += delta_seconds;
}


/*
 *
 */
void ESPTime::print() {
  Serial.printf("%lu.%lu\n", _seconds, _millis);
}


/*
 *
 */
void ESPTime::getNTPepoch(char * server) {
  // TODO verbosity
  Serial.printf("%04d: %s\n",
                millis(),
                "NTP sending request");

  uint16_t request_start = millis();
  _udp.begin(UDP_LOCAL_PORT);
  IPAddress timeServerIP;
  WiFi.hostByName(server, timeServerIP);
  sendNTPpacket(timeServerIP);  // send an NTP packet to a time server
  delay(200);  // wait to see if a reply is available

  int cb = _udp.parsePacket();
  if (!cb) {
    Serial.printf("%04d: %s\n",
                  millis(),
                  "NTP no packet received");
    return;
  } else {
    _udp.read(_packetBuffer, NTP_PACKET_SIZE);

    uint32_t ntp_seconds = (uint32_t)_packetBuffer[40] << 24
                         | (uint32_t)_packetBuffer[41] << 16
                         | (uint32_t)_packetBuffer[42] <<  8
                         | (uint32_t)_packetBuffer[43] <<  0;

    uint32_t ntp_millis = (uint32_t)_packetBuffer[44] << 24
                        | (uint32_t)_packetBuffer[45] << 16
                        | (uint32_t)_packetBuffer[46] << 8
                        | (uint32_t)_packetBuffer[47] << 0;

    ntp_seconds -= 2208988800UL;
    ntp_millis = ((uint64_t) ntp_millis * 1000) >> 32;

    uint16_t request_time = millis() - request_start;
    Serial.printf("%04d: %s: %d\n", millis(), "NTP request time", request_time);

    this->_seconds = ntp_seconds;
    this->_millis = ntp_millis;
  }
}



/*
 * send an NTP request to the time server at the given address
 * helper function to build NTP packet
 */
void ESPTime::sendNTPpacket(IPAddress& address) {
  memset(_packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  _packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  _packetBuffer[1] = 0;     // Stratum, or type of clock
  _packetBuffer[2] = 6;     // Polling Interval
  _packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  _packetBuffer[12]  = 49;
  _packetBuffer[13]  = 0x4E;
  _packetBuffer[14]  = 49;
  _packetBuffer[15]  = 52;

  _udp.beginPacket(address, NTP_PORT); //NTP requests are to port 123
  _udp.write(_packetBuffer, NTP_PACKET_SIZE);
  _udp.endPacket();
}


/*
 *
 */
int32_t ESPTime::delta(ESPTime *time) {
  int32_t delta;

  delta = this->getSeconds();
  delta -= time->getSeconds();
  delta *= 1000;

  delta += this->getMillis();
  delta -= time->getMillis();

  return(delta);
}
