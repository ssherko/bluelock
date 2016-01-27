#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include "messagebus.h"
#include "logger.h"
//#include "util.h"


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
      exit(EXIT_ERR_DBUS_SET_MSG);
    }

    if(!dbus_connection_send_with_reply(connection, lock_message, &pending, -1)){
      log_event("<bus_send_message>", "Error sending DBus message (Lock)", ERRO);
      exit(EXIT_ERR_DBUS_SND_MSG);
    }

    if(pending == NULL){
      log_event("<bus_send_message>", "Pending call set to (NULL)", ERRO);
      exit(EXIT_ERR_DBUS_PCALL);
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
    int set_to = 0; //false

    unlock_message = dbus_message_new_method_call(destination,object,interface,method);
    if(unlock_message == NULL){
      log_event("<bus_send_message>", "Error setting DBus message", ERRO);
      exit(EXIT_ERR_DBUS_SET_MSG);
    }

    dbus_message_iter_init_append(unlock_message,&argument);
    if(!dbus_message_iter_append_basic(&argument, DBUS_TYPE_BOOLEAN, &set_to)){
      log_event("<bus_send_message>", "Error setting argument for DBus message (Unlock)", ERRO);
      exit(EXIT_ERR_DBUS_SET_ARGS);
    }

    if(!dbus_connection_send_with_reply(connection, unlock_message, &pending, -1)){
      log_event("<bus_send_message>", "Error sending DBus message (Unlock)", ERRO);
      exit(EXIT_ERR_DBUS_SND_MSG);
    }

    if(pending == NULL){
      log_event("<bus_send_message>", "Pending call set to (NULL)", ERRO);
      exit(EXIT_ERR_DBUS_PCALL);
    }

    dbus_connection_flush(connection);
    dbus_message_unref(unlock_message);
    pending = NULL;

    // If the screen has been turned off (due to being locked for a 'long' period of time)
    // issue a call to 'SimulateUserActivity'
    method = "SimulateUserActivity";
    DBusMessage* simulate_activity_message;

    simulate_activity_message = dbus_message_new_method_call(destination,object,interface,method);
    if(simulate_activity_message == NULL){
      log_event("<bus_send_message>", "Error setting DBus message", ERRO);
      exit(EXIT_ERR_DBUS_SET_MSG);
    }

    if(!dbus_connection_send_with_reply(connection, simulate_activity_message, &pending, -1)){
      log_event("<bus_send_message>", "Error sending DBus message (SimulateUserActivity)", ERRO);
      exit(EXIT_ERR_DBUS_SND_MSG);
    }

    if(pending == NULL){
      log_event("<bus_send_message>", "Pending call set to (NULL)", ERRO);
      exit(EXIT_ERR_DBUS_PCALL);
    }

    dbus_connection_flush(connection);
    dbus_message_unref(simulate_activity_message);

  }

}