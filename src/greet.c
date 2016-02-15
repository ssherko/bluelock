#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "greet.h"
#include "logger.h"

void create_greeting(char* greet_id, char* username){
	char greetfile_path[100];
	char greetfile_id[18];
	int i = 0;

	for(; i < 18; i++){
		if(greet_id[i] == ':'){
			greetfile_id[i] = '_';
			continue;
		}

		greetfile_id[i] = greet_id[i];
	}

	strcat_mult_str(7, greetfile_path, DATA_PATH, GREETFOLD_NAME, "/", greetfile_id, ".wav");
	int status;

	char command[150];
	strcat_mult_str(9, command, TTS_ENGINE, ENGINE_ARG, greetfile_path, " \"Hello ", username, "\"", SUPPRESS);
	status = system(command);

	if(status < 0){
		char msg[50];
		strcpy(msg,"Couldn't create greeting for user: ");
		strcat(msg,username);
		log_event("<create_greeting>", msg, WARN);
	}

}

void delete_greeting(char* greet_id){
	char greetfile_path[100];
	char greetfile_id[18];
	int i = 0;

	for(; i < 18; i++){
		if(greet_id[i] == ':'){
			greetfile_id[i] = '_';
			continue;
		}

		greetfile_id[i] = greet_id[i];
	}
	
	strcat_mult_str(7, greetfile_path, DATA_PATH, GREETFOLD_NAME, "/", greetfile_id, ".wav");
	int status = remove(greetfile_path);

	if(status < 0){
		log_event("<delete_greeting>", "Something went wrong when deleting the greeting", WARN);
	}

}

void play_greeting(char* greet_id){
	char greetfile_path[100];
	char greetfile_id[18];
	int i = 0;

	for(; i < 18; i++){
		if(greet_id[i] == ':'){
			greetfile_id[i] = '_';
			continue;
		}

		greetfile_id[i] = greet_id[i];
	}

	strcat_mult_str(7, greetfile_path, DATA_PATH, GREETFOLD_NAME, "/", greetfile_id, ".wav");
	int status;

	char command[150];
	strcat_mult_str(5, command, PLAYB_ENGINE, greetfile_path, SUPPRESS);
	status = system(command);

	if(status < 0){
		log_event("<play_greeting>", "Something went wrong when playing the greeting", WARN);
	}
	
}
