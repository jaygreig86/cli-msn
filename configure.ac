#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
VERSION=2.3.2
PACKAGE=CLI-MSN
ADDRESS=james@mor-pah.net
AC_INIT(CLI-MSN, 2.3.2, james@mor-pah.net)
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
AM_SANITY_CHECK
AC_CONFIG_SRCDIR([libs/md5.c])
AC_CONFIG_HEADER([src/headers/config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(ncurses, wprintw, [SHARE_LIBNCURSES=1],
                         [AC_MSG_ERROR([Unable to find NCurses. Please download from http://www.gnu.org/software/ncurses/ncurses.html])])

AC_CHECK_LIB(curl, curl_easy_setopt, [SHARE_LIBCURL=1],
			 [AC_MSG_ERROR([Unable to find LibCurl.  Please download from http://curl.haxx.se/])])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_CHECK_FUNCS([bzero gethostbyname memset select setenv socket strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
