#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(ws9xxd, 0.0.4, BUG-REPORT-ADDRESS)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h memory.h stdlib.h sys/ioctl.h unistd.h signal.h getopt.h stdarg.h stdio.h ctype.h sys/stat.h sys/un.h sys/types.h time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([memset select])

# Check for big endian host:
AC_C_BIGENDIAN([HOST_IS_BIGENDIAN="1"], [HOST_IS_BIGENDIAN="0"], [HOST_IS_BIGENDIAN="0"])
AM_CONDITIONAL(IS_BIGENDIAN, test x$HOST_IS_BIGENDIAN = x1)
AC_ARG_VAR([HOST_IS_BIGENDIAN], [host endian flag])

AC_CONFIG_FILES([Makefile
                src/Makefile
				examples/Makefile])
AC_OUTPUT
