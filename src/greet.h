/*
	
	'greet' is responsible for creating/maintaining audio greetings for 
	users. It uses the pico2wave program to generate the file.

	
*/

#ifndef GREET_H
#define GREET_H

#include "util.h"

#define TTS_ENGINE "pico2wave " // Don't remove the whitespace
#define ENGINE_ARG "-w "
#define PLAYB_ENGINE "aplay "
#define SUPPRESS " 2> /dev/null"

extern char* DATA_PATH;
extern int GREET_USER;

void create_greeting(char* greet_id, char* username);
void delete_greeting(char* greet_id);
void play_greeting(char* greet_id);

#endif