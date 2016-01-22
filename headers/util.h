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
	bool disp_help;
	bool scan;
	bool run_daemon;

} cmd_args;

typedef struct {
	int sth;
}global_vars;

void print_header();
cmd_args parse_cmd(int argc, char** argv);
void print_help();
global_vars fetch_global_vars();

#endif