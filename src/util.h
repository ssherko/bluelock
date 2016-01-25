/*
	
	'util' provides functions for dealing with the 
	program's runtime variables and parameters,
	maintaining the program's data folder path.

*/

#ifndef UTIL_H
#define UTIL_H
#include <unistd.h>
#include <stdlib.h>

typedef enum { FALSE, TRUE } bool;

typedef struct {
	bool add;
	bool del;
	bool list_keys;
	bool list_params;
	bool edit_param;
	bool disp_help;
	bool scan;
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
#endif