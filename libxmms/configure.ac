AC_PREREQ([2.72])

AC_INIT([libxmms],[3.0.2],[xmms-devel@xmms.org])
AC_CANONICAL_HOST

AC_MSG_NOTICE([XMMS modernized build system])
    
AC_MSG_CHECKING([for macOS])
case "$host" in
  *darwin*)
    HAVE_MACOS=yes
    ;;
  *)  
    HAVE_MACOS=no
    ;;
esac
AC_MSG_RESULT([$HAVE_MACOS])

AM_CONDITIONAL([HAVE_MACOS], [test "x$HAVE_MACOS" = "xyes"])

AC_SUBST([OBJCFLAGS])
AC_PROG_OBJC

AC_CONFIG_SRCDIR([configfile.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_PREFIX_PROGRAM([xmms])

dnl Version numbers
LIBXMMS_MAJOR_VERSION=3
LIBXMMS_MINOR_VERSION=0
LIBXMMS_MICRO_VERSION=2
LIBXMMS_VERSION=$LIBXMMS_MAJOR_VERSION.$LIBXMMS_MINOR_VERSION.$LIBXMMS_MICRO_VERSION

AC_SUBST([LIBXMMS_MAJOR_VERSION])
AC_SUBST([LIBXMMS_MINOR_VERSION])
AC_SUBST([LIBXMMS_MICRO_VERSION])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

LT_INIT

PKG_PROG_PKG_CONFIG

dnl Replace old GTK1 macros
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.40])
PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.0])

AC_SUBST([GLIB_CFLAGS])
AC_SUBST([GLIB_LIBS])
AC_SUBST([GTK_CFLAGS])
AC_SUBST([GTK_LIBS])

dnl Compiler warnings
if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall -Wpointer-arith"
fi

AC_CHECK_HEADERS([
sched.h
sys/sched.h
unistd.h
sys/time.h
time.h
])

AC_CHECK_FUNCS([
sched_setscheduler
nanosleep
])

AC_CONFIG_FILES([
Makefile
])
AC_OUTPUT

