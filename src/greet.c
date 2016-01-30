#include <string.h>

#include "util.h"
#include "greet.h"

bool create_greeting(char* greet_id, char* username){
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
	strcat(command, " \"Hello ");
	strcat(command, username);
	strcat(command, "\"");

	status = system(command);

	return (bool)(status == 1);

}

bool delete_greeting(char* greet_id){
	return FALSE;
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

	system(command);
	
}
