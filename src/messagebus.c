#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messagebus.h"
#include "logger.h"

/*
Sends messages over DBUS.

@param message The message to send (LOCK/UNLOCK).
*/
void bus_send_message(char* message){
  DBusError error;
  DBusConnection* connection;
  dbus_error_init(&error);
  connection = dbus_bus_get(DBUS_BUS_SESSION, &error);

  if(dbus_error_is_set(&error)){
    char* log_msg = (char*)malloc(sizeof(char)*LOG_LEN);
    strcpy(log_msg,"Error connecting to DBus ");
    strcat(log_msg, error.message);
    log_event("<bus_send_message>", log_msg, ERRO);
    dbus_error_free(&error);
  }

  if(connection == NULL){
    log_event("<bus_send_message>", "Error establishing connection", ERRO);
    exit(6);
  }

 /******************************
  *   SEND THE "LOCK" MESSAGE  *
  ******************************/

  if(strcmp(message,"LOCK") == 0){
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
      exit(7); // TODO: Document exit statuses
    }

    if(!dbus_connection_send_with_reply(connection, lock_message, &pending, -1)){
      log_event("<bus_send_message>", "Error sending DBus message (Lock)", ERRO);
      exit(9);
    }

    if(pending == NULL){
      log_event("<bus_send_message>", "Pending call set to (NULL)", ERRO);
      exit(10);
    }

    dbus_connection_flush(connection);
    dbus_message_unref(lock_message);

  }


  /*******************************
  *   SEND THE "UNLOCK" MESSAGE  *
  ********************************/

  if(strcmp(message,"UNLOCK") == 0){
    DBusMessage* unlock_message;
    DBusPendingCall* pending;
    DBusMessageIter argument;

    // method call -> dest=org.gnome.ScreenSaver serial=2 path=/org/gnome/ScreenSaver; interface=org.gnome.ScreenSaver; member=SetActive(false)
    char* destination = "org.gnome.ScreenSaver";
    char* object = "/org/gnome/ScreenSaver";
    char* interface = "org.gnome.ScreenSaver";
    char* method = "SetActive";
    int set_to = 0;

    unlock_message = dbus_message_new_method_call(destination,object,interface,method);
    if(unlock_message == NULL){
      log_event("<bus_send_message>", "Error setting DBus message", ERRO);
      exit(7);
    }

    dbus_message_iter_init_append(unlock_message,&argument);
    if(!dbus_message_iter_append_basic(&argument, DBUS_TYPE_BOOLEAN, &set_to)){
      log_event("<bus_send_message>", "Error setting argument for DBus message (Unlock)", ERRO);
      exit(8);
    }

    if(!dbus_connection_send_with_reply(connection, unlock_message, &pending, -1)){
      log_event("<bus_send_message>", "Error sending DBus message (Unlock)", ERRO);
      exit(9);
    }

    if(pending == NULL){
      log_event("<bus_send_message>", "Pending call set to (NULL)", ERRO);
      exit(10);
    }

    dbus_connection_flush(connection);
    dbus_message_unref(unlock_message);

  }

}