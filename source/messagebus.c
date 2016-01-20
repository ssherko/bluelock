#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/messagebus.h"
#include "../headers/logger.h"

void bus_send_message(char* message){
	DBusError error;
	DBusConnection* connection;
  dbus_error_init(&error);
  connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
   	
  if (dbus_error_is_set(&error)) {
 		char* log_msg = (char*)malloc(sizeof(char)*LOG_LEN);
    strcpy(log_msg, "Error connecting to DBus ");
    strcat(log_msg, error.message);
    log_event("<bus_send_message>", log_msg, ERRO);
 		dbus_error_free(&error);
	}

	if(connection == NULL){
		log_event("<bus_send_message>", "Error establishing connection", ERRO);
		exit(6);
	}
  // A connection to DBUS has been established
	
  if(strcmp(message,"LOCK") == 0 ){ // lock the screen
		
		DBusMessage* lock_message;
		DBusPendingCall* pending;
		
		// method call -> dest=org.gnome.ScreenSaver serial=2 path=/org/gnome/ScreenSaver; interface=org.gnome.ScreenSaver; member=Lock
		char* destination = "org.gnome.ScreenSaver";
		char* object = "/org/gnome/ScreenSaver";
		char* interface = "org.gnome.ScreenSaver";
		char* method = "Lock";

		lock_message = dbus_message_new_method_call(destination,object,interface,method);
		if(lock_message == NULL){
			log_event("<bus_send_message>", "Error setting DBus message", ERRO);
			exit(7); // TODO document this.
		}

 		if(!dbus_connection_send_with_reply (connection, lock_message, &pending, -1)){
 			log_event("<bus_send_message>", "Error sending DBus message (Lock)", ERRO);
      exit(9); // and this.
 		}

 		
 		if(pending == NULL) { 
      log_event("<bus_send_message>", "Pending call set to (NULL)", ERRO);
      exit(10);// and this .. 
 		}
 		
    dbus_connection_flush(connection);
    dbus_message_unref(lock_message);
	}


  if(strcmp(message,"UNLOCK") == 0){
  	printf("UNLOCK\n");
  }

}