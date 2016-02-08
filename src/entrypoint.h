/*
	
	'entrypoint' holds the main() function of the bluelock application.
	Also, it declares 'global' runtime parameters for the daemon.


*/

#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

/****************************************
*       DAEMON RUNTIME PARAMETERS       *
*****************************************/

// Maximum number of devices to be discovered nearby.
// e.g: If 10 bluetooth devices are nearby, only NR_MAX_DISCOVERED_DEVICES 
// are going to be picked, arbitrarily.
int NR_MAX_DISCOVERED_DEVICES = 5;

// Due to the discovery of nearby bluetooth devices not being *very* reliable 
// (i.e.: the device might be on, but not picked up as being nearby by
// Bluelock), a 'history' of previous 'discovery attempts' is stored. The datastructure
// is defined in 'daemon.h' and corresponds to a circular array. The length of the 
// array is defined here as MAX_HISTORY_LEN.
int MAX_HISTORY_LEN = 3; 

// The time (TIME_PER_SCAN*1.28 seconds) to wait for nearby devices to respond to 
// a discovery message. 
int TIME_PER_SCAN = 2;

// Whenever a valid bluetooth key is found, the daemon waits for SLEEP_TIME seconds before
// scanning for nearby keys again.
int SLEEP_TIME = 1;

// Play an audio clip each time the machine is unlocked due to a device being nearby
int GREET_USER = 0;

/****************************************
*     PERSISTING-DATA ABSOLUTE PATHS    *
*****************************************/
// Note: They're set during runtime.

// The path of the folder where the application persists its data.
// Should be ~/.bluelock
char* DATA_PATH;

// The path to the keystore file (storing valid bluetooth keys)
// Should be DATA_PATH + '/settings'
char* KEY_STORE_PATH;

// The path to the log file.
// Should be DATA_PATH + '/logs'
char* LOGS_PATH;

#endif
