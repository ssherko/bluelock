/*
	
	'messagebus' is responsible for sending LOCK and UNLOCK messages
	over DBUS IPC mechanism.
	

*/

#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H

/****************************************
*            EXIT STATUSES              *
*****************************************/
#define EXIT_ERR_DBUS_CONNECT 6
#define EXIT_ERR_DBUS_SET_MSG 7
#define EXIT_ERR_DBUS_SND_MSG 8
#define EXIT_ERR_DBUS_PCALL 9
#define EXIT_ERR_DBUS_SET_ARGS 10

void bus_send_message(char* message);

#endif