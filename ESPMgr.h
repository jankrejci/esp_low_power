/*
 * ESPMgr.h
 */


#ifndef ESPMgr_h
#define ESPMgr_h


/*** INCLUDES ***/

#include <FS.h>
#include <ESP8266WiFi.h>
//#include <WiFiUDP.h>
#include <DNSServer.h>
#include <PubSubClient.h>


/*** DEFINES ***/

#define CONFIG_FILE           "esp.conf"
#define DELIMITER             ','
#define LINE_BUFFER           300

#define RESET_PIN             D5
#define SERIAL_SPEED          115200

#define CONFIG_LOADED         0
#define FS_ERROR              1
#define NO_CONFIG_FILE        2
#define CONFIG_READ_FAIL      3

#define TYPE_IP               0
#define TYPE_BSSID            1

#define WIFI_CONNECTED        0
#define WIFI_CONNECTION_FAIL  1

#define WIFI_DELAY            100 // time in ms, how often to check wifi status while connecting
#define WIFI_STAT_MSG         3   // time in s, how often to print status msg while connecting
#define WIFI_TIMEOUT          10  // time in s, how long to try to connect to wifi

#define RST_DEEP_SLEEP        5

#define UDP_LOCAL_PORT        6969

#define NTP_PORT              123
#define NTP_PACKET_SIZE       48

#define MQTT_CONNECTED        0
#define MQTT_FAILED           1

#define KEY                   0
#define NICE_NAME             1
#define VALUE                 2

#define VALUE_LEN_MAX         100
#define NAME_LEN_MAX          30

#define PARAMS_MAX            50

// structure to define custom parameter
struct Param {
  char key[NAME_LEN_MAX];
  char nice_name[VALUE_LEN_MAX];
  char value[VALUE_LEN_MAX];
};


/*** CLASES ***/

/*
 *
 */
class ESPTime {
  public:
    ESPTime();
    ~ESPTime();

    void      setTime(uint32_t seconds, uint32_t millis);
    uint32_t  getSeconds();
    uint32_t  getMillis();
    void      addMillis(int32_t millis);
    void      print();
    void      getNTPepoch(char *server);
    void      sendNTPpacket(IPAddress& address);
    int32_t  delta(ESPTime *time);

  private:
    // TODO reduce to uint16
    uint32_t      _seconds;
    uint32_t      _millis;
    byte          _packetBuffer[NTP_PACKET_SIZE];
    WiFiUDP       _udp;

};


/*
 *
 */
class ESPMgr {
  public:
    ESPMgr();
    ~ESPMgr();

    void      begin();
    char*     readParamValue(const char *key);
    void      setVerbosity(uint8_t verbosity);
    void      rmConfig();
    void      loadConfig();
    void      setSleepTime(uint32_t sleep_time);
    void      printParams();
    void      printFile(char* filename);
    void      halt(uint32_t sleep_time);
    void      halt();
    void      connectWifi();
    void      mqttPublish(char* message);
    ESPTime   now();

  private:
    /*** VARIABLES ***/
    struct Param _params[PARAMS_MAX] = {
      // key, nice_name, default value
      // {"wifi_ssid",     "SSID",         "WiFi VNT"},
      // {"wifi_pass",     "Password",     "vnttnv321"},
      {"wifi_ssid",     "SSID",         "Skynet Gate"},
      {"wifi_pass",     "Password",     "velkasvestka"},
      {"wifi_chan",     "Channel",      ""},
      {"wifi_bssid",    "BSSID",        ""},
      {"wifi_ip",       "IP",           ""},
      {"wifi_mask",     "Mask",         ""},
      {"wifi_gate",     "Gateway",      ""},
      {"wifi_dns",      "DNS",          "8.8.8.8"},
      {"wifi_time",     "Last update",  ""},

      {"mqtt_server",   "Server",       "skynjari.cz"},
      {"mqtt_port",     "Port",         "1883"},
      {"mqtt_user",     "User name",    ""},
      {"mqtt_pass",     "Password",     ""},
      {"mqtt_id",       "ID",           "esp_sleep"},
      {"mqtt_topic",    "Topic",        "esp/test"},

      {"time_server",   "NTP",          "tak.cesnet.cz"},
      {"time_wake_s",   "Wake time s",  ""},
      {"time_wake_ms",  "Wake time ms", ""},
      {"time_zone",     "Timezone",     "2"},
      {"",              "",             ""}  // last key to define end
    };

    uint8_t       _verbosity  = 1;
    uint32_t      _sleep_time = 1e6;
    uint8_t       _reset_info = 0;

    WiFiClient    _espClient;
    PubSubClient  _mqttClient;

    ESPTime       _sync_time;

    /*** FUNCTIONS ***/
    int       loadParams(char* filename);
    char**    splitLine(char* line);
    void      saveParams(char* filename);
    void      writeValue(const char* key, const char* value);
    int       writeParam(char* key, char* nice_name, char* value);
    int       connectSaved();
    int       connectDHCP();
    int       connectionCheck();
    unsigned char*     charToAddr(char* address, int type);
    int       charToInt(char* number, char* format);
    void      printAddr(unsigned char* addr, int type);
    void      writeWifiParams();
    void      resetWifiParams();
    void      syncTimeNTP();
    void      syncTimeSaved();


    uint32_t  getLastUpdateDelta();
    void      writeWifiTimeParam();
    void      writeTimeParam();
    int       mqttConnect();
};


#endif
