#include <unistd.h>
#include <stdlib.h>
#include "devlist_handler.h"
/*
*/
#ifndef UTIL_H
#define UTIL_H

typedef enum { FALSE, TRUE } bool;

typedef struct {
	bool add;
	bool del;
	char* del_val;
	bool list_keys;
	bool disp_help;
	bool scan;
	bool run_daemon;

} cmd_args;

void print_header();
cmd_args parse_cmd(int argc, char** argv);
void print_help();
void free_key(key_device_t* key);

#endif