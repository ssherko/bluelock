/*
	
	'util' provides functions for dealing with the 
	program's runtime variables and parameters,
	maintaining the program's data folder path.

*/

#ifndef UTIL_H
#define UTIL_H

/****************************************
*            EXIT STATUSES              *
*****************************************/
#define EXIT_ERR_OPEN_BT_DEV 1
#define EXIT_ERR_OPEN_SOCKET 2
#define EXIT_ERR_ALLOC_MEM 3
#define EXIT_ERR_HQI_INQUIRY 4
#define EXIT_ERR_OPEN_KEYSTORE 5
// 6 - 10 are DBUS RELATED ERRORS, see "messagebus.h"
#define EXIT_ERR_OPEN_LOGS 11
#define EXIT_ERR_OPEN_SETTINGS 12
#define EXIT_ERR_CREAT_DATAFOLD 13
#define EXIT_ERR_CREAT_LOGFILE 14
#define EXIT_ERR_CREAT_GREETFOLD 15

#define EXIT_SIG_RECV 0

#define LOGFOLD_NAME "/logs"
#define SETTINGSFOLD_NAME "/settings"
#define GREETFOLD_NAME "/greetings"
#define BLUELOCKFOLD_NAME "/.bluelock"
#define KEYSTORE_NAME "/keystore"

typedef enum { FALSE, TRUE } bool;

typedef struct {
	bool add;
	bool del;
	bool list_keys;
	bool list_params;
	bool edit_param;
	bool disp_help;
	bool list_logs;
	bool run_daemon;
} cmd_args;

// See 'entrypoint.h' for their definition.
extern int NR_MAX_DISCOVERED_DEVICES;
extern int MAX_HISTORY_LEN;
extern int SLEEP_TIME;
extern int TIME_PER_SCAN;

cmd_args parse_cmd(int argc, char** argv);
char* check_persistent_data();
void persist_settings(char* data_path);
int get_parameter_value(char* variable_string);
void fetch_settings(char* data_path);
int validate_value(int value);

//Printing functions.
void list_params();
void print_header();
void print_help();
void print_logs(char* logs_path);

void signal_handler(int signal_no);

#endif