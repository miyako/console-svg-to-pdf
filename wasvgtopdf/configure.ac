#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([wasvgtopdf], [0.1], [keisuke.miyako@4d.com])
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([wasvg.h])
# AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([cairo], [cairo_save])
AC_CHECK_LIB([rsvg-2], [rsvg_cleanup])
AC_CHECK_LIB([gobject-2.0], [g_object_unref])

# Checks for header files.
AC_CHECK_HEADERS([locale.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset sqrt strchr strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
