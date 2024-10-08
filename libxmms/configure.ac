dnl Process this file with autoconf to produce a configure script.

AC_INIT


cflags_set=${CFLAGS+set}
AC_CONFIG_AUX_DIR([autoconf])
AC_CONFIG_MACRO_DIR([autoconf-m4])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
#m4_ifdef([AC_CONFIG_HEADERS], [AC_CONFIG_HEADERS([config.h])], [AC_CONFIG_HEADERS([config.h])])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_MAINTAINER_MODE], [AM_MAINTAINER_MODE])
AM_GNU_GETTEXT([external])




AC_CONFIG_SRCDIR([configfile.c])

AM_CONFIG_HEADER(config.h)

dnl libtool version:
LIBXMMS_MAJOR_VERSION=3
LIBXMMS_MINOR_VERSION=0
LIBXMMS_MICRO_VERSION=2
LIBXMMS_VERSION=$LIBXMMS_MAJOR_VERSION.$LIBXMMS_MINOR_VERSION.$LIBXMMS_MICRO_VERSION

AM_INIT_AUTOMAKE(libxmms, $LIBXMMS_VERSION)

AC_SUBST(LIBXMMS_MAJOR_VERSION)
AC_SUBST(LIBXMMS_MINOR_VERSION)
AC_SUBST(LIBXMMS_MICRO_VERSION)

AC_PREFIX_PROGRAM(xmms)
dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AM_PROG_LIBTOOL
AM_ENABLE_STATIC
AM_ENABLE_SHARED

XMMS_FUNC_POSIX(sched_setscheduler nanosleep)

AM_PATH_GLIB(1.2.2,,AC_MSG_ERROR([*** GLIB >= 1.2.2 not installed - please install first ***]))
AM_PATH_GTK(1.2.2,,AC_MSG_ERROR([*** GTK+ >= 1.2.2 not installed - please install first ***]),gthread)

if test -n "$GCC"
then
  CFLAGS="$CFLAGS -Wall -Wpointer-arith"
fi

m4_warn([obsolete],
[Update your code to rely only on HAVE_SYS_TIME_H,
then remove this warning and the obsolete code below it.
All current systems provide time.h; it need not be checked for.
Not all systems provide sys/time.h, but those that do, all allow
you to include it and time.h simultaneously.])dnl
AC_CHECK_HEADERS_ONCE([sys/time.h])
# Obsolete code to be removed.
if test $ac_cv_header_sys_time_h = yes; then
  AC_DEFINE([TIME_WITH_SYS_TIME],[1],[Define to 1 if you can safely include both <sys/time.h>
	     and <time.h>.  This macro is obsolete.])
fi
# End of obsolete code.


AC_CHECK_HEADERS(sched.h)
AC_CHECK_HEADERS(sys/sched.h)
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(sys/time.h)

AC_CONFIG_FILES([Makefile
])
AC_OUTPUT
