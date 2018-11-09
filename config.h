/*
 * config.h
 */

#ifndef CONFIG_H


/*** INCLUDES ***/

#include "FS.h"


/*** DEFINES ***/

#define CONFIG_H

// verbosity of the debug output: 0 - no msg, 1 - status msg only, 2 - all msg
#define VERBOSITY         1
#define SERIAL_SPEED      115200

#define SLEEP_TIME        10e6  // deep sleep time

#define CONFIG_FILE       "/config.json"
#define DELIMITER         ','
#define LINE_BUFFER       300

#define CONFIG_LOADED     0
#define FS_ERROR          1
#define NO_CONFIG_FILE    2
#define CONFIG_READ_FAIL  3

#define KEY               0
#define NICE_NAME         1
#define VALUE             2

// structure to define custom parameter
#define VALUE_LEN   100
#define NAME_LEN    30

struct Param {
  char key[NAME_LEN];
  char nice_name[VALUE_LEN];
  char value[VALUE_LEN];
};


/*** FUNCTIONS ***/

char* readValue(const char* key);
void writeValue(const char* key, const char* value);
int writeParam(char* key, char* nice_name, char* value);
void saveParams(char* filename);
int loadParams(char* filename);
void printFile(char* filename);
void printParams();
void printString(char* text);
char** splitLine(char* line);
void cleanConfig();
void rmConfig();
void initSerial();
void initFS();


#endif
