/*
	
	'greet' is responsible for creating/maintaining greetings for 
	users
	

*/

#ifndef GREET_H
#define GREET_H

#include "util.h"

#define TTS_ENGINE "pico2wave " // Don't remove the whitespace
#define ENGINE_ARG "-w "
#define PLAYB_ENGINE "aplay "

extern char* DATA_PATH;
extern int GREET_USER;

bool create_greeting(char* greet_id, char* username);
bool delete_greeting(char* greet_id);
void play_greeting(char* greet_id);

#endif