#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "greet.h"
#include "logger.h"

void create_greeting(char* greet_id, char* username){
	char greetfile_path[100];
	strcpy(greetfile_path, DATA_PATH);
	strcat(greetfile_path, GREETFOLD_NAME);
	char greetfile_id[18];
	int i = 0;

	for(; i < 18; i++){
		if(greet_id[i] == ':'){
			greetfile_id[i] = '_';
			continue;
		}

		greetfile_id[i] = greet_id[i];
	}
	strcat(greetfile_path, "/");
	strcat(greetfile_path, greetfile_id);
	strcat(greetfile_path, ".wav");

	int status;

	char command[150];
	strcpy(command, TTS_ENGINE);
	strcat(command, ENGINE_ARG);
	strcat(command, greetfile_path);
	strcat(command, " \"Hello , ");
	strcat(command, username);
	strcat(command, "\"");
	strcat(command, SUPPRESS);

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
	strcpy(greetfile_path, DATA_PATH);
	strcat(greetfile_path, GREETFOLD_NAME);
	char greetfile_id[18];
	int i = 0;

	for(; i < 18; i++){
		if(greet_id[i] == ':'){
			greetfile_id[i] = '_';
			continue;
		}

		greetfile_id[i] = greet_id[i];
	}
	strcat(greetfile_path, "/");
	strcat(greetfile_path, greetfile_id);
	strcat(greetfile_path, ".wav");

	int status = remove(greetfile_path);

	if(status < 0){
		log_event("<delete_greeting>", "Something went wrong when deleting the greeting", WARN);
	}

}

void play_greeting(char* greet_id){
	char greetfile_path[100];
	strcpy(greetfile_path, DATA_PATH);
	strcat(greetfile_path, GREETFOLD_NAME);
	char greetfile_id[18];
	int i = 0;

	for(; i < 18; i++){
		if(greet_id[i] == ':'){
			greetfile_id[i] = '_';
			continue;
		}

		greetfile_id[i] = greet_id[i];
	}
	strcat(greetfile_path, "/");
	strcat(greetfile_path, greetfile_id);
	strcat(greetfile_path, ".wav");

	int status;

	char command[150];
	strcpy(command, PLAYB_ENGINE);
	strcat(command, greetfile_path);
	strcat(command, SUPPRESS);

	status = system(command);

	if(status < 0){
		log_event("<play_greeting>", "Something went wrong when playing the greeting", WARN);
	}
	
}
