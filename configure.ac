#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([bluelock], [1.0])
AC_CONFIG_SRCDIR([src/entrypoint.c])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([dbus-1],[dbus_bus_get],[],[
	echo "lib DBUS-1 required, NOT FOUND. (apt-get install libdbus-1-dev libdbus-glib-1-dev)"
	exit -1
])

PKG_CHECK_MODULES([DBUS], [dbus-1])

AC_CHECK_LIB([bluetooth], [hci_get_route],[],[
	echo "lib BLUETOOTH required, NOT FOUND. (apt-get install libbluetooth-dev bluez-utils)"
	exit -1
])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/socket.h unistd.h bluetooth/bluetooth.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([memset strerror])

AC_OUTPUT(Makefile src/Makefile man/Makefile)
