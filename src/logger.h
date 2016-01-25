/*
	'logger' is responsible for logging events during the execution of the program.
	A log entry adheres to the format:

	log : <TIME> <LEVEL> <FUNCTION_NAME> <MESSAGE> <ADDITIONAL_INFO (if any)> 
	where
		<TIME> DD/MM/YYYY HH:MM:SS format
		<LEVEL>
			0 -> INF0
			1 -> WARN
			2 -> ERRO

*/

#ifndef LOGGER_H
#define LOGGER_H
#define LOG_LEN 140

#define INFO 0
#define WARN 1
#define ERRO 2

extern char* LOGS_PATH; // defined in 'entrypoint.h'

void log_event(char* function_name, char* message_body,int level);
#endif