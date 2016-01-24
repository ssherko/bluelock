/*
*/
#ifndef UTIL_H
#define UTIL_H
#include <unistd.h>
#include <stdlib.h>

typedef enum { FALSE, TRUE } bool;

typedef struct {
	bool add;
	bool del;
	char* del_val;
	bool list_keys;
	bool list_params;
	bool edit_param;
	bool disp_help;
	bool scan;
	bool run_daemon;

} cmd_args;

extern int NR_MAX_DISCOVERED_DEVICES;
extern int MAX_HISTORY_LEN;
extern int INQUIRY_TIME;
extern int TIME_PER_SCAN;

void print_header();
cmd_args parse_cmd(int argc, char** argv);
void print_help();
char* check_persistent_data();
void persist_settings(char* data_path);
int get_parameter_value(char* variable_string);
void fetch_settings(char* data_path);
void list_params();
int validate_value(int value);

#endif