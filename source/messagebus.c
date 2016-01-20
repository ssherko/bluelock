#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/messagebus.h"

void bus_send_message(char* message){
	DBusError error;
	DBusConnection* connection;
   	dbus_error_init(&error);

   	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
   	
   	if (dbus_error_is_set(&error)) {
   		printf("<bus_send_message>Error connecting to DBUS: %s\n", error.message);
   		dbus_error_free(&error);
  	}

  	if(connection == NULL){
  		perror("<bus_send_message> Error establishing connection: ");
  		exit(6);
  	}

  	if(strcmp(message,"LOCK") == 0 ){ // lock the screen
  		
  		DBusMessage* lock_message;
  		DBusMessageIter args;
  		DBusPendingCall* pending;
  		char* param = "";
  		
  		// method call -> dest=org.gnome.ScreenSaver serial=2 path=/org/gnome/ScreenSaver; interface=org.gnome.ScreenSaver; member=Lock
  		char* destination = "org.gnome.ScreenSaver";
  		char* object = "/org/gnome/ScreenSaver";
  		char* interface = "org.gnome.ScreenSaver";
  		char* method = "Lock";

  		lock_message = dbus_message_new_method_call(destination,object,interface,method);
  		if(lock_message == NULL){
  			perror("<bus_send_message> Error setting DBUS message: ");
  			exit(7); // TODO document this.
  		}

  		
  		dbus_message_iter_init_append(lock_message, &args);
  		if(!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param)){
  			perror("<bus_send_message> Error appending arguments to DBUS message: ");
  			exit(8); // document this too.
   		}

   		if(!dbus_connection_send_with_reply (connection, lock_message, &pending, -1)){
   			perror("<bus_send_message> Error sending DBUS LOCK message: "); 
      		exit(9); // and this.
   		}

   		
   		if(pending == NULL) { 
      		perror("<bus_send_message> Pending Call set to NULL: "); 
      		exit(10);// and this .. 
   		}
   		
   		dbus_connection_flush(connection);
		dbus_message_unref(lock_message);
  	}

  	if(strcmp(message,"UNLOCK") == 0){
  		printf("UNLOCK\n");
  	}

}