#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H
#include "util.h"

// Default settings.
int NR_MAX_DISCOVERED_DEVICES = 5;
int MAX_HISTORY_LEN = 3;
int INQUIRY_TIME = 2;
int TIME_PER_SCAN = 2;

char* DATA_PATH;
char* KEY_STORE_PATH;
char* LOGS_PATH;

#endif
