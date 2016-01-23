INC = -I/usr/include/dbus-1.0/ -I/usr/lib/i386-linux-gnu/dbus-1.0/include/
AM_CFLAGS = -Wall $(INC) -lbluetooth -ldbus-1

bin_PROGRAMS = bluelock
bluelock_SOURCES = daemon.c daemon.h\
				   devlist_handler.c devlist_handler.h\
				   entrypoint.c entrypoint.h\
				   logger.c logger.h \
				   messagebus.c messagebus.h\
				   scanner.c scanner.h\
				   util.c util.h