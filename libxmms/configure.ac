# Process this file with autoconf to produce a configure script.

AC_INIT([libxmms],[3.0.2],[your-email@example.com])

# Enable automatic archive creation if available
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

LT_INIT

# Define auxiliary directories
AC_CONFIG_AUX_DIR([autoconf])
AC_CONFIG_MACRO_DIR([autoconf-m4])

# Enable silent rules for cleaner build output
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Enable maintainer mode for developers
m4_ifdef([AM_MAINTAINER_MODE], [AM_MAINTAINER_MODE])

# Initialize gettext for translations
AM_GNU_GETTEXT([external])

# Check for required libraries
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.30])
PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.0])

# Specify the source file for version control
AC_CONFIG_SRCDIR([configfile.c])

# Configure header files
AC_CONFIG_HEADERS([config.h])

# Versioning for libxmms
LIBXMMS_MAJOR_VERSION=3
LIBXMMS_MINOR_VERSION=0
LIBXMMS_MICRO_VERSION=2
LIBXMMS_VERSION=$LIBXMMS_MAJOR_VERSION.$LIBXMMS_MINOR_VERSION.$LIBXMMS_MICRO_VERSION

AM_INIT_AUTOMAKE([foreign])

# Substitute versioning info in the Makefile
AC_SUBST([LIBXMMS_MAJOR_VERSION])
AC_SUBST([LIBXMMS_MINOR_VERSION])
AC_SUBST([LIBXMMS_MICRO_VERSION])

# Specify program to be built
AC_PREFIX_PROGRAM([xmms])

# Checks for required programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Enable libtool for shared and static library building
AC_ENABLE_STATIC([])
AC_ENABLE_SHARED([])

# Check for POSIX functions
AC_CHECK_FUNCS([sched_setscheduler nanosleep])

# Enable extra warnings if GCC is being used
if test -n "$GCC"; then
  CFLAGS="$CFLAGS -Wall -Wpointer-arith"
fi

# Check for system headers
AC_CHECK_HEADERS([sched.h sys/sched.h unistd.h sys/time.h])

# Output files to be generated
AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT

