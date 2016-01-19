#include <stdio.h>
#include <unistd.h>
#include "../headers/util.h"

void print_help(){
	printf("Usage: bluelock [-a][-d <key_id>] [-l][-h]\n");
	printf("\t-a\t\tAdd a new bluetooth key and exit\n");
	printf("\t-d <key_id>\tDelete the bluetooth key with ID <key_id> and exit\n");
	printf("\t-l\t\tList existing bluetooth keys and exit\n");
	printf("\t-h\t\tDisplay this message and exit\n");
	printf("If no arguments are provided, 'bluelock' will run as a daemon\n");
}

void print_header(){
}

cmd_args parse_cmd(int argc,char** argv){
	
	cmd_args args;
	args.add = FALSE;
	args.del = FALSE;
	args.list_keys = FALSE;
	args.disp_help = FALSE;
	args.run_daemon = TRUE;

	int c;

	while((c = getopt(argc, argv, "adlh")) != -1){
		switch(c){
			case 'a':
				args.add = TRUE;
				break;

			case 'd':
				args.del = TRUE;
				break;

			case 'l':
				args.list_keys = TRUE;
				break;

			case 'h':
				args.disp_help = TRUE;
				break;

			default:
				break;
		}
	}

	return args;

}


void free_key(key_device_t* key){
	
}