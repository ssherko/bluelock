#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>

#include "util.h"
#include "logger.h"

/*
Parses the command line arguments into a struct (defined in 'util.h'),
which is then used to control the execution flow of the program.

@param argc The number of command line arguments.
@param argv The command line arguments (a list of strings).
@return args The actual struct.
*/
cmd_args parse_cmd(int argc,char** argv){
	
	cmd_args args;
	args.add = FALSE;
	args.del = FALSE;
	args.list_keys = FALSE;
	args.list_params = FALSE;
	args.edit_param = FALSE;
	args.disp_help = FALSE;
	args.list_logs = FALSE;
	args.run_daemon = TRUE;

	int c;

	while((c = getopt(argc, argv, "adkselh")) != -1){
		switch(c){
			case 'a':
				args.add = TRUE;
				break;

			case 'd':
				args.del = TRUE;
				break;

			case 'k':
				args.list_keys = TRUE;
				break;

			case 's':
				args.list_params = TRUE;
				break;

			case 'e':
				args.edit_param = TRUE;
				break;

			case 'l':
				args.list_logs = TRUE;
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
Checks whether the path were the data should be persisted(~/.bluelock/) exists. 
It if does, it returns the data path (absolute) else it creates the folder.

@return data_path The path of the folder where program data is to be persisted.
*/
char* check_persistent_data(){
	char* data_path = (char*)malloc(sizeof(char)*50);
	if(data_path == NULL){
		perror("<check_persistent_data> Error allocating memory: ");
		exit(EXIT_ERR_ALLOC_MEM);
	}

	char* home_path = getenv("HOME");
	strcpy(data_path, home_path);
	strcat(data_path,BLUELOCKFOLD_NAME);

	int status = mkdir(data_path, S_IRWXO | S_IRWXU);
	
	if(status == -1){
		if(errno == EEXIST)
			return data_path;
		perror("<check_persistent_data> Error creating data folder: ");
		exit(EXIT_ERR_CREAT_DATAFOLD);
	}

	printf("First time run ...\n");
	printf("Initializing persistent data folder and default settings in '%s'.\n", data_path);
	persist_settings(data_path);

	//creating log file
	char log_path[50];
	strcpy(log_path,data_path);
	strcat(log_path, LOGFOLD_NAME);
	status = creat(log_path, S_IRWXO | S_IRWXU);
	if(status == -1){
		perror("<check_persistent_data> Error creating log file: ");
		exit(EXIT_ERR_CREAT_LOGFILE);
	}

	//create greeting subfolder
	char greeting_folder[50];
	strcpy(greeting_folder, data_path);
	strcat(greeting_folder,GREETFOLD_NAME);
	status = mkdir(greeting_folder, S_IRWXO | S_IRWXU);
	if(status == -1){
		perror("<check_persistent_data> Error creating greetings folder: ");
		exit(EXIT_ERR_CREAT_GREETFOLD);
	}

	return data_path;

}

/*
Persist the daemon runtime parameters/settings into a file named
'settings' under the data path folder ('~/.bluelock/').
See 'entrypoint.h' for the settings definition.

@param data_path The path of the data folder.
*/
void persist_settings(char* data_path){
	
	char* settings_path = (char*)malloc(sizeof(char)*50);
	if(settings_path == NULL){
		log_event("<persist_settings>", "Error allocating memory",ERRO);
		exit(EXIT_ERR_ALLOC_MEM);
	}
	strcpy(settings_path, data_path);
	strcat(settings_path, SETTINGSFOLD_NAME);
	
	FILE* settings_file = fopen(settings_path, "w");
	if(settings_file == NULL){
		log_event("<persist_settings>", "Error opening settings file", ERRO);
		exit(EXIT_ERR_OPEN_SETTINGS);
	}
	
	fprintf(settings_file, "%s=%d\n", "NR_MAX_DISCOVERED_DEVICES", NR_MAX_DISCOVERED_DEVICES);
	fprintf(settings_file, "%s=%d\n", "MAX_HISTORY_LEN", MAX_HISTORY_LEN);
	fprintf(settings_file, "%s=%d\n", "SLEEP_TIME", SLEEP_TIME);
	fprintf(settings_file, "%s=%d\n", "TIME_PER_SCAN", TIME_PER_SCAN);
	fprintf(settings_file, "%s=%d\n", "GREET_USER", GREET_USER);

	fclose(settings_file);
	free(settings_path);

}

/*
Retrieves the value from a "<VARIABLE_NAME>=<VARIABLE_VALUE>" string.
It is expected that <VARIABLE_VALUE> is a string representation of an
integer. The function parses said representation into an integer and
returns it.

@param variable_string The string where to get the value from.
@return value Integer representation of the value.
*/
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

/*
Parses the 'settings' file under the program data folder 
and sets the corresponding 'global' variables appropriately.

@param data_path The absolute path of the program's data folder.
*/
void fetch_settings(char* data_path){
	char* settings_path = (char*)malloc(sizeof(char)*50);
	if(settings_path == NULL){
		log_event("<fetch_settings>", "Error allocating memory",ERRO);
		exit(EXIT_ERR_ALLOC_MEM);
	}
	strcpy(settings_path, data_path);
	strcat(settings_path, SETTINGSFOLD_NAME);

	FILE* settings_file = fopen(settings_path, "r");
	if(settings_file == NULL){
		log_event("<fetch_settings>", "Error opening settings file", ERRO);
		exit(EXIT_ERR_OPEN_SETTINGS); // document this
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

			case 'S':
				SLEEP_TIME = value;
				break;

			case 'T':
				TIME_PER_SCAN = value;
				break;

			case 'G':
				GREET_USER = value;
				break;

			default:
				log_event("<fetch_settings>", "Unkown setting in settings file",WARN);
				break;
		}
	}

	fclose(settings_file);
	free(settings_path);
}

/*
Validates the new value that a user is trying to assign to any of the 
daemon parameters.
*/
int validate_value(int value, int which_param){
	switch(which_param){
		case 4: // GREET_USER
			return 1;

		default:
			return (value <= 99999) && (value > 0);

	}
	return 0;
}

/****************************************
*              PRINT STUFF              *
*****************************************/

void print_help(){
	printf("Usage: bluelock [-a][-d][-k][-s][-e][-l][-h]\n");
	printf("\t-a\t\tAdd a new bluetooth key and exit\n");
	printf("\t-d\t\tDelete an existing bluetooth key and exit\n");
	printf("\t-k\t\tList existing bluetooth keys and exit\n");
	printf("\t-s\t\tList daemon parameters and exit\n");
	printf("\t-e\t\tEdit daemon parameters and exit\n");
	printf("\t-l\t\tList logs and exit\n");
	printf("\t-h\t\tDisplay this message and exit\n");
	printf("If no arguments are provided, 'bluelock' will run as a daemon\n");
}

void print_header(){
	printf("Bluelock 2016 (v%s) - Use bluetooth devices as keys.\n", VERSION_STR);
}

void list_params(){
	printf("Current daemon parameters: \n");
	printf("\t - [0] NR_MAX_DISCOVERED_DEVICES: %d\n", NR_MAX_DISCOVERED_DEVICES);
	printf("\t - [1] MAX_HISTORY_LEN: %d\n", MAX_HISTORY_LEN);
	printf("\t - [2] SLEEP_TIME: %d\n", SLEEP_TIME);
	printf("\t - [3] TIME_PER_SCAN: %d\n", TIME_PER_SCAN);
	printf("\t - [4] GREET_USER: ");
	
	if(GREET_USER){
		printf("True\n");
	}else{
		printf("False\n");
	}

}

void print_logs(char* logs_path){
	FILE* log_file = fopen(logs_path, "r");
	if(log_file == NULL){
		perror("<print_logs> Error opening log file: ");
		exit(EXIT_ERR_OPEN_LOGS);
	}
	int lines = 0;
	size_t read;
	size_t length = 0;
	char* log_string;
	while((read = getline(&log_string, &length, log_file)) != -1){
		printf(" - %s", log_string);
		lines++;
	}

	fclose(log_file);

	if(lines == 0){
		printf("\t - No logs to display\n");

	}

}

void signal_handler(int signal_no){
	switch(signal_no){
		
		case SIGINT:
			log_event("<signal_handler>", "Stopping daemon (SIGINT received)", INFO);
			exit(EXIT_SIG_RECV);
			break;

		case SIGTERM:
			log_event("<signal_handler>", "Stopping daemon (SIGTERM received)", INFO);
			exit(EXIT_SIG_RECV);
			break;

		default:
			break;

	}
}

void strcat_mult_str(char* dst, char* src1, ...){
	va_list string_list;

	strcpy(dst,src1);
	va_start(string_list, src1);
	
	char* next_str;
	while(TRUE){
		next_str = va_arg(string_list, char*);
		if(!next_str)
			break;
		strcat(dst,next_str);
	}

	va_end(string_list);
}