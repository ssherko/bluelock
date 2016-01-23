#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([bluelock], [1.0])
AC_CONFIG_SRCDIR([src/entrypoint.c])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/socket.h unistd.h dbus/dbus.h bluetooth/bluetooth.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([memset strerror])

AC_OUTPUT(Makefile src/Makefile)
