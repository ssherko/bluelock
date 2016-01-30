AM_CFLAGS = -Wall ${DBUS_CFLAGS}
AM_LFLAGS = -lbluetooth ${DBUS_LIBS}

bin_PROGRAMS = bluelock
bluelock_SOURCES = daemon.c daemon.h\
				   devlist_handler.c devlist_handler.h\
				   entrypoint.c entrypoint.h\
				   logger.c logger.h \
				   messagebus.c messagebus.h\
				   scanner.c scanner.h\
				   util.c util.h\
				   greet.c greet.h