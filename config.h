/*
 * config.h
 */

#ifndef CONFIG_H


/*** DEFINES ***/

#define CONFIG_H

#define CONFIG_FILE       "/config.json"
#define DELIMITER         ','
#define LINE_BUFFER       300

#define CONFIG_OK         0
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


/*** FUNCTIONS */

char* getValue(const char* key);
struct Param readParam(const char* key);
int writeParam(char* key, char* nice_name, char* value);
void saveParams(char* filename);
int loadParams(char* filename);
void printFile(char* filename);
void printParams();
char** splitLine(char* line);


#endif
