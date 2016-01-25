/*
	
	'messagebus' is responsible for sending LOCK and UNLOCK messages
	over DBUS IPC mechanism.
	

*/

#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

void bus_send_message(char* message);

#endif