#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

void bus_send_message(char* message);

#endif