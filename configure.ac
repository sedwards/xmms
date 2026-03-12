

AC_INIT([xmms],[1.2.6],[xmms-devel@xmms.org])
AC_CONFIG_SRCDIR([xmms/main.c])
AC_CONFIG_HEADERS([config.h xmms/i18n.h])

dnl === Feature templates required by autoheader ===

AH_TEMPLATE([HAVE_MACOS_AUDIO],
  [Define to 1 if building with macOS audio backend support.])

AH_TEMPLATE([HAVE_METAL],
  [Define to 1 if building with Metal GPU support on macOS.])

AH_TEMPLATE([HAVE_OPENGL],
  [Define to 1 if building with OpenGL plugin support.])

AH_TEMPLATE([HAVE_LIBZ],
  [Define to 1 if libz (zlib) support is available.])

AC_CONFIG_MACRO_DIRS([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CANONICAL_HOST
AC_PREFIX_PROGRAM([xmms])

AC_MSG_NOTICE([XMMS modernized build system])

AC_MSG_CHECKING([for macOS])
case "$host_os" in
  darwin*)
    HAVE_MACOS=yes
    ;;
  *)
    HAVE_MACOS=no
    ;;
esac
AC_MSG_RESULT([$HAVE_MACOS])

AM_CONDITIONAL([HAVE_MACOS], [test "x$HAVE_MACOS" = "xyes"])

AC_SUBST([OBJCFLAGS])

#AC_MSG_NOTICE([
#Building for macOS. Make sure dependencies are installed:

#  brew install autoconf automake libtool pkg-config gtk+3
#])
#    ;;
#esac

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_OBJC

LT_INIT

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.40])
PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.0])

AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AC_CHECK_HEADERS([
unistd.h
fcntl.h
sys/ioctl.h
limits.h
sched.h
wchar.h
])

AC_CHECK_FUNCS([
mkdtemp
sched_get_priority_max
iconv_open
])

AC_CHECK_LIB([pthread],[pthread_create],
    PTHREAD_LIBS="-lpthread",
    PTHREAD_LIBS="")

AC_SUBST(PTHREAD_LIBS)

AC_ARG_ENABLE([macos-audio],
  AS_HELP_STRING([--enable-macos-audio],[Enable CoreAudio output]),
  enable_macos_audio=$enableval,
  enable_macos_audio=no)

if test "x$enable_macos_audio" = "xyes"; then
  AC_DEFINE([HAVE_MACOS_AUDIO],[1])
fi

AM_CONDITIONAL([HAVE_MACOS_AUDIO],
  [test "x$enable_macos_audio" = "xyes"])

AC_ARG_ENABLE([metal],
  AS_HELP_STRING([--enable-metal],[Enable Metal visualization]),
  enable_metal=$enableval,
  enable_metal=no)

if test "x$enable_metal" = "xyes"; then
  AC_DEFINE([HAVE_METAL],[1])
fi

AM_CONDITIONAL([HAVE_METAL],
  [test "x$enable_metal" = "xyes"])

AC_ARG_ENABLE([opengl],
  AS_HELP_STRING([--disable-opengl],[Disable OpenGL plugins]),
  enable_opengl=$enableval,
  enable_opengl=yes)

if test "x$enable_opengl" = "xyes"; then
  PKG_CHECK_MODULES([OPENGL],[gl],have_opengl=yes,have_opengl=no)
else
  have_opengl=no
fi

if test "x$have_opengl" = "xyes"; then
  AC_DEFINE([HAVE_OPENGL],[1])
fi

AM_CONDITIONAL([HAVE_OPENGL],
  [test "x$have_opengl" = "xyes"])


AC_CHECK_LIB([z], [inflate],
[
  Z_LIBS="-lz"
  AC_DEFINE([HAVE_LIBZ], [1], [Define if zlib support is available])
],
[
  Z_LIBS=""
])

AC_SUBST([Z_LIBS])

case "$host_os" in
  darwin*)
    have_oss=no
  ;;
  *)
    AC_CHECK_HEADERS([sys/soundcard.h machine/soundcard.h])

    if test "$ac_cv_header_sys_soundcard_h" = yes ||
       test "$ac_cv_header_machine_soundcard_h" = yes; then
      have_oss=yes
    else
      have_oss=no
    fi
  ;;
esac

AM_CONDITIONAL([HAVE_OSS],[test "x$have_oss" = "xyes"])

plugindir="$libdir/xmms"
AC_SUBST(plugindir)

localedir=${datadir}/locale
AC_SUBST(localedir)

XMMS_DEFINES="-DDATA_DIR=\\\"$datadir/xmms\\\" \
-DPLUGIN_DIR=\\\"$plugindir\\\" \
-DLOCALEDIR=\\\"$localedir\\\""

AC_SUBST(XMMS_DEFINES)

PLUGIN_LDFLAGS='-module -avoid-version -export-symbols-regex "get_.plugin_info"'
AC_SUBST(PLUGIN_LDFLAGS)

AC_CONFIG_SUBDIRS([libxmms])

AC_CONFIG_FILES([
Makefile
xmms/Makefile
Input/Makefile
Output/Makefile
Effect/Makefile
General/Makefile
Visualization/Makefile
po/Makefile.in
intl/Makefile
])

AC_OUTPUT

echo ""
echo "XMMS configuration"
echo ""
echo " prefix:              $prefix"
echo " GTK3 support:        yes"
echo " OpenGL plugins:      $have_opengl"
echo " Metal plugins:       $enable_metal"
echo " macOS CoreAudio:     $enable_macos_audio"
echo " OSS support:         $have_oss"
echo ""

