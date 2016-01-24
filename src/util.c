#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include "util.h"
#include "logger.h"

void print_help(){
	printf("Usage: bluelock [-a][-d][-l][-s][-e][-h]\n");
	printf("\t-a\t\tAdd a new bluetooth key and exit\n");
	printf("\t-d\t\tDelete the bluetooth key with ID <key_id> and exit\n");
	printf("\t-l\t\tList existing bluetooth keys and exit\n");
	printf("\t-s\t\tList daemon parameters and exit\n");
	printf("\t-e\t\tEdit daemon parameters and exit\n");
	printf("\t-h\t\tDisplay this message and exit\n");
	printf("If no arguments are provided, 'bluelock' will run as a daemon\n");
}

void print_header(){
	printf("Bluelock 2016 - Use bluetooth devices as keys.\n");
}

cmd_args parse_cmd(int argc,char** argv){
	
	cmd_args args;
	args.add = FALSE;
	args.del = FALSE;
	args.list_keys = FALSE;
	args.list_params = FALSE;
	args.edit_param = FALSE;
	args.disp_help = FALSE;
	args.run_daemon = TRUE;

	int c;

	while((c = getopt(argc, argv, "adlseh")) != -1){
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

			case 's':
				args.list_params = TRUE;
				break;

			case 'e':
				args.edit_param = TRUE;
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


/*
Checks whether the path were the data
should be persisted(~/.bluelock/) exists.

*/
char* check_persistent_data(){
	char* data_path = (char*)malloc(sizeof(char)*50);
	if(data_path == NULL){
		perror("<check_persistent_data> Error allocating memory: ");
		exit(3);
	}

	char* home_path = getenv("HOME");
	strcpy(data_path, home_path);
	strcat(data_path,"/.bluelock");

	int status = mkdir(data_path, S_IRWXO | S_IRWXU);
	
	if(status == -1){
		if(errno == EEXIST)
			return data_path;
		perror("<check_persistent_data> Error creating data folder: ");
		exit(22);
	}

	persist_settings(data_path);
	return data_path;
}

// settings defined in entrypoint.h
void persist_settings(char* data_path){
	
	char* settings_path = (char*)malloc(sizeof(char)*50);
	if(settings_path == NULL){
		log_event("<persist_settings>", "Error allocating memory",ERRO);
		exit(3);
	}
	strcpy(settings_path, data_path);
	strcat(settings_path, "/settings");
	
	FILE* settings_file = fopen(settings_path, "w");
	if(settings_file == NULL){
		log_event("<persist_settings>", "Error opening settings file", ERRO);
		exit(23); // document this
	}
	
	fprintf(settings_file, "%s=%d\n", "NR_MAX_DISCOVERED_DEVICES", NR_MAX_DISCOVERED_DEVICES);
	fprintf(settings_file, "%s=%d\n", "MAX_HISTORY_LEN", MAX_HISTORY_LEN);
	fprintf(settings_file, "%s=%d\n", "INQUIRY_TIME", INQUIRY_TIME);
	fprintf(settings_file, "%s=%d\n", "TIME_PER_SCAN", TIME_PER_SCAN);

	fclose(settings_file);
	free(settings_path);

}

int get_parameter_value(char* variable_string){
	int i = 0;
	while(variable_string[i] != '='){
		i++;
	}
	i++; //skip over '='
	char value_string[5];
	int j = 0;
	while(variable_string[i] != '\0'){
		value_string[j] = variable_string[i];
		i++;
		j++;
	}

	int value = strtol(value_string, '\0', 10);
	return value;

}
void fetch_settings(char* data_path){
	char* settings_path = (char*)malloc(sizeof(char)*50);
	if(settings_path == NULL){
		log_event("<fetch_settings>", "Error allocating memory",ERRO);
		exit(3);
	}
	strcpy(settings_path, data_path);
	strcat(settings_path, "/settings");

	FILE* settings_file = fopen(settings_path, "r");
	if(settings_file == NULL){
		log_event("<fetch_settings>", "Error opening settings file", ERRO);
		exit(23); // document this
	}

	int c;
	char variable_string[40];
	while((c = fscanf(settings_file,"%s",variable_string)) != -1){
		int value = get_parameter_value(variable_string);
		switch(variable_string[0]){
			case 'N':
				NR_MAX_DISCOVERED_DEVICES = value;
				break;

			case 'M':
				MAX_HISTORY_LEN = value;
				break;

			case 'I':
				INQUIRY_TIME = value;
				break;

			case 'T':
				TIME_PER_SCAN = value;
				break;

			default:
				log_event("<fetch_settings>", "Unkown setting in settings file",WARN);
				break;
		}
	}

	fclose(settings_file);
	free(settings_path);
}

void list_params(){
	printf("Current daemon parameters: \n");
	printf("\t - [0] NR_MAX_DISCOVERED_DEVICES: %d\n", NR_MAX_DISCOVERED_DEVICES);
	printf("\t - [1] MAX_HISTORY_LEN: %d\n", MAX_HISTORY_LEN);
	printf("\t - [2] INQUIRY_TIME: %d\n", INQUIRY_TIME);
	printf("\t - [3] TIME_PER_SCAN: %d\n", TIME_PER_SCAN);
}

int validate_value(int value){
	return (value < 99999) && (value > 0);
}