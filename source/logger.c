#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "../headers/devlist_handler.h"
#include "../headers/logger.h"

void log_event(char* function_name, char* message_body,int level){
	FILE* log_file = fopen("config/logs", "a");
	
	if(log_file == NULL){
		perror("<log_event> couldn't open log file: "); // log_event("log_event", "Couldn't open log file",ERRO);// what could go wrong ?
		exit(21);
	}

	char* event_str = (char*)malloc(sizeof(char)*LOG_LEN);
	time_t happened_at = time(NULL);
	struct tm* local_time = localtime(&happened_at);
	
	strftime(event_str, sizeof(char)*DATE_LEN , DATE_FORMAT, local_time);


	char* level_str;
	char* additional = "";
	if(level == INFO){
		level_str = "INFO";
	}

	if(level == WARN){
		level_str = "WARN";
	}

	if(level == ERRO){
		level_str = "ERRO";
		additional = strerror(errno);
	}

	strcat(event_str, " ");
	strcat(event_str, level_str);
	strcat(event_str, ": ");

	strcat(event_str, function_name);
	strcat(event_str, " ");
	strcat(event_str, message_body);
	strcat(event_str, " ");
	strcat(event_str, additional);

	fprintf(log_file,"%s\n", event_str);
	
	free(event_str);
	fclose(log_file);
}