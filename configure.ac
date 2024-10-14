# Process this file with autoconf to produce a configure script.

AC_INIT([xmms],[0.0.1],[https://updatez.org])

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

AC_SUBST([GTK_CFLAGS])
AC_SUBST([GTK_LIBS])

# Specify the source file for version control
AC_CONFIG_SRCDIR([libxmms/configfile.c])

# Configure header files
AC_CONFIG_HEADERS([config.h])

# Versioning for libxmms
LIBXMMS_MAJOR_VERSION=3
LIBXMMS_MINOR_VERSION=0
LIBXMMS_MICRO_VERSION=2
LIBXMMS_VERSION=$LIBXMMS_MAJOR_VERSION.$LIBXMMS_MINOR_VERSION.$LIBXMMS_MICRO_VERSION

#AM_INIT_AUTOMAKE([foreign])
AM_INIT_AUTOMAKE([-Wall foreign])

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

AC_PROG_CC
AC_SEARCH_LIBS([strerror],[cposix])
dnl AC_PROG_CXX
AC_C_BIGENDIAN
AC_C_INLINE
AC_C_CONST

AC_DISABLE_STATIC([])

# Enable libtool for shared and static library building
#AC_ENABLE_STATIC([])
AC_ENABLE_SHARED([])

# Check for POSIX functions
AC_CHECK_FUNCS([sched_setscheduler nanosleep])


#m4_ifde([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
#m4_ifdef([AC_CONFIG_HEADERS], [AC_CONFIG_HEADERS([config.h])], [AC_CONFIG_HEADERS([config.h])])
#m4_ifdef([AM_MAINTAINER_MODE], [AM_MAINTAINER_MODE])



AC_ARG_ENABLE( one-plugin-dir,
[  --enable-one-plugin-dir  use a single plugin dir [default=no]],,
		enable_one_plugin_dir="no")

AC_ARG_ENABLE( user-plugin-dir,
[  --disable-user-plugin-dir disable per-user plugin dir],,
		enable_user_plugin_dir="yes")

AC_ARG_WITH(dev-dsp, 
  [  --with-dev-dsp=path     Path to OSS DSP data pipe, default is /dev/dsp.],
	dev_dsp="$withval", dev_dsp="/dev/dsp")
AC_DEFINE_UNQUOTED(DEV_DSP,"$dev_dsp", [Path to OSS DSP, really just a data pipe, default is /dev/dsp.])

AC_ARG_WITH(dev-mixer, 
  [  --with-dev-mixer=path   Path to OSS sound mixer, default is /dev/mixer.],
	dev_mixer="$withval", dev_mixer="/dev/mixer")
AC_DEFINE_UNQUOTED(DEV_MIXER,"$dev_mixer", [Path to OSS mixer, default is /dev/mixer.])

AC_ARG_WITH(cdda-device, 
  [  --with-cdda-device=path Path to default cdaudio device.],
  AC_DEFINE_UNQUOTED(CDDA_DEVICE,"$withval", [Path to default cdaudio device.]))

AC_ARG_WITH(cdda-dir, 
  [  --with-cdda-dir=path    Path to default cdaudio directory.],
  AC_DEFINE_UNQUOTED(CDDA_DIRECTORY,"$withval", [Path to default cdaudio directory.]))


AC_ARG_ENABLE(gtk2,
              [  --disable-gtk2          Do not look for GTK+ 2.0 libraries],
              enable_gtk2=$enableval,
              enable_gtk2="no")

AC_ARG_ENABLE(gtk3,
              [  --enable-gtk3           Enable GTK+ 3.0 (Experimental)],
              enable_gtk3=$enableval,
              enable_gtk3="yes")

AC_ARG_ENABLE(gtk4,
              [  --enable-gtk4           Enable GTK+ 4.0 (Experimental)],
              enable_gtk4=$enableval,
              enable_gtk4="no")

AC_ARG_ENABLE(gtkport,
              [  --disable-gtkport       Disable compiling the GTK+ port],
              enable_gtkport=$enableval,
              enable_gtkport="yes")

AC_ARG_ENABLE(gtk_warnings,
              [  --enable-gtk-warnings   So you can see what is deprecated in GTK2...],
              gtk_warnings=$enableval,
              gtk_warnings='np')

AC_ARG_ENABLE(debug,
              [  --enable-debug      Print debug info to std output],
              enable_debug=$enableval,
              enable_debug="no")

AC_ARG_WITH(libxml2,
              [  --with-libxml2          use libxml2 [default=libxml1 unless GTK2 is enabled]],
              with_libxml2="no") 


# Check for required libraries
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.30])
PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.0])

AC_PATH_PROG(XMMS_PATH,xmms,no)

#AM_PROG_AS
CCASFLAGS=""
LIBS_save=$LIBS
LIBS="$LIBS"
SM_LIBS=""
AC_CHECK_LIB(SM, SmcOpenConnection,[
AC_DEFINE(WITH_SM)
SM_LIBS="-lSM -lICE"])
AC_SUBST(SM_LIBS)
LIBS=$LIBS_save

AC_PATH_X

CFLAGS_save=$CFLAGS
LIBS_save=$LIBS
CFLAGS="$CFLAGS $GTK_CFLAGS"
LIBS="$LIBS"

CFLAGS=$CFLAGS_save
LIBS=$LIBS_save
AC_SUBST(VM_LIBS)


dnl *** OpenGL
AC_ARG_ENABLE( opengl,
[  --disable-opengl        Disable OpenGL plugin(s) [default=yes]],,
		enable_opengl="yes")

if test "x$enable_opengl" = xyes; then
	LIBS_save=$LIBS
	LIBS="$LIBS"
	OPENGL_LIBS=error
	AC_CHECK_LIB(GL, glBegin, OPENGL_LIBS="-lGL")

	if test "x$OPENGL_LIBS" = xerror; then
		AC_CHECK_LIB(MesaGL, glBegin,OPENGL_LIBS="-lMesaGL")
	fi

	CPPFLAGS_save=$CPPFLAGS
	CPPFLAGS="$GTK_CFLAGS $CPPFLAGS"
	have_opengl_headers=yes
	AC_CHECK_HEADERS(GL/gl.h GL/glx.h,,have_opengl_headers=no)

	if test "x$OPENGL_LIBS" = xerror || test "x$have_opengl_headers" = xno; then
		have_opengl=no
	else
		have_opengl=yes
	fi

	CPPFLAGS=$CPPFLAGS_save
	LIBS=$LIBS_save
else
	AC_MSG_RESULT([*** Disabling OpenGL plugins per user request ***])
	have_opengl=no
fi

AC_SUBST(OPENGL_LIBS)

AM_CONDITIONAL(HAVE_OPENGL, test "x$have_opengl" = xyes)


dnl *** zlib
LIBS_save=$LIBS
Z_LIBS=""
AC_CHECK_LIB(z, inflate, [Z_LIBS="-lz"
                          AC_DEFINE(HAVE_LIBZ,,[Define if zlib is available])])
AC_SUBST(Z_LIBS)
LIBS=$LIBS_save


dnl *** Look for the XML libraries
AC_PATH_PROG(HAVEXML, xml${XMLVER}-config, $PATH)
if test ! -z "$HAVEXML"; then
    XML_CFLAGS=`$HAVEXML --cflags`
    XML_PREFIX=`$HAVEXML --prefix`

        if "$HAVEXML" --help|grep libtool-libs > /dev/null; then
                XML_LIBTOOL=`$HAVEXML --libtool-libs`
                if test -f "$XML_LIBTOOL"; then
                        . "$XML_LIBTOOL"
                        XML_LIBS="$dependency_libs"
                else
                        XML_LIBS="-lm"
                fi
        elif "$HAVEXML" --libs | grep thread > /dev/null; then
                XML_LIBS="$PTHREAD -lm"
        fi

    CFLAGS="$CFLAGS $XML_CFLAGS"
    if test "$STATIC" = ""; then
        LIBS="$LIBS $XML_STATIC -L$XML_PREFIX/lib$LIBSUFFIX -lxml${XMLVER} $BDYNAMIC $XML_LIBS"
        GUI_LIBS="$GUI_LIBS $XML_STATIC -L$XML_PREFIX/lib$LIBSUFFIX -lxml${XMLVER} $BDYNAMIC $XML_LIBS"
        CARBON_LIBS="$CARBON_LIBS -L$XML_PREFIX/lib$LIBSUFFIX -lxml${XMLVER} $XML_LIBS"
    else
        LIBS="$LIBS -l${XMLVER} $XML_LIBS"
        GUI_LIBS="$GUI_LIBS -l${XMLVER} $XML_LIBS"
        CARBON_LIBS="$CARBON_LIBS -l${XMLVER} $XML_LIBS"
    fi
else
    AC_MSG_ERROR([*** xml${XMLVER}-config not found. You need a working libxml${XMLVER} installation.])
fi

XMMS_GTK=""

dnl *** disable gtk warnings
if test x"$gtk_warnings" = x"yes"; then
   CPPFLAGS="$CPPFLAGS -DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_DEPRECATED -DGTK_DISABLE_SINGLE_INCLUDES"
fi

AC_SUBST(XMMS_GTK)

dnl *** esound
AC_ARG_ENABLE( esd,
[  --disable-esd           Disable esound output plugin [default=enabled]],,
		enable_esd="yes")

if test "x$enable_esd" = xyes; then
	AM_PATH_ESD(0.2.8, 
	   [have_esd=yes 
	    AC_CHECK_LIB(esd, esd_get_latency, 
	       AC_DEFINE(HAVE_ESD_GET_LATENCY,,
		  [Define if libesd has the esd_get_latency function]))], 
	   have_esd=no)
else
	AC_MSG_RESULT([*** esound plugin disabled per user request ***])
	have_esd=no
fi

AM_CONDITIONAL(HAVE_ESD,test "x$have_esd" = xyes)

dnl *** Ogg Vorbis
AC_ARG_ENABLE( vorbis,
[  --disable-vorbis        Disable Ogg Vorbis input plugin [default=enabled]],,
		enable_vorbis="yes")

if test "x$enable_vorbis" = xyes; then
	AM_PATH_OGG(have_ogg=yes, have_ogg=no)
	AM_PATH_VORBIS(have_vorbis=yes, have_vorbis=no)
	if test "x$have_vorbis" = xyes; then
	  AC_MSG_CHECKING(if vorbis is sufficiently new)
	  CFLAGS_save=$CFLAGS
	  LIBS_save=$LIBS
	  CFLAGS="$CFLAGS $VORBIS_CFLAGS"
	  LIBS="$LIBS $VORBIS_LIBS"
	  AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <vorbis/codec.h>]], [[int main(void)
		 {
		       vorbis_info info;
		       ogg_packet op;
                       vorbis_packet_blocksize(&info, &op);
                       return 0;
		 }]])],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT([no (need atleast 1.0rc1)])
		 have_vorbis=no])
	  CFLAGS=$CFLAGS_save
	  LIBS=$LIBS_save
	fi
	have_oggvorbis=no
	if test "x$have_ogg" = xyes
	then
	  if test "x$have_vorbis" = xyes
	  then
	    have_oggvorbis=yes
	  fi
	fi
else
	AC_MSG_RESULT([*** Ogg Vorbis plugin disabled per user request ***])
	have_oggvorbis=no
fi
AM_CONDITIONAL(HAVE_OGGVORBIS,test $have_oggvorbis = yes)

HAVE_ORBIT=no

dnl Set of available languages.
ALL_LINGUAS="af az bg ca cs da de el eo es eu fr ga gl hr hu id it ja ko lt lv nn nl no pl pt pt_BR ro ru sk sr sv tg th tr uk vi wa zh_CN.GB2312 zh_TW"

localedir=${datadir}/locale
AC_SUBST(localedir)

dnl If {type}_PLUGINS is defined, it should be a comma or space separated
dnl list of plugins that should be built, e.g.,
dnl INPUT_PLUGINS="mpg123,cdaudio cdin".  If variable is not defined, all
dnl buildable plugins are compiled.  This stuff makes life easier for
dnl automated builds that want to disable certain plugins.

for class in EFFECT GENERAL INPUT OUTPUT VISUALIZATION; do
    value=`eval echo "\\$${class}_PLUGINS"`
    if test "x${value}" = x; then
       eval ${class}_PLUGINS="\$\(ALL_PLUGINS\)"
    else
       value=`echo ${value} | sed -e 's/,/ /g'`
       eval ${class}_PLUGINS="\${value}"
    fi
done

AC_SUBST(EFFECT_PLUGINS)
AC_SUBST(GENERAL_PLUGINS)
AC_SUBST(INPUT_PLUGINS)
AC_SUBST(OUTPUT_PLUGINS)
AC_SUBST(VISUALIZATION_PLUGINS)


dnl 3DNow! instructions support
dnl ========================
AC_ARG_ENABLE(3dnow,
	[  --enable-3dnow	  build with 3DNow! instructions support],
			USE_3DNOW=$enableval, USE_3DNOW=no)
if test "x$USE_3DNOW" = xyes; then
  AC_DEFINE(USE_3DNOW)
fi
AM_CONDITIONAL(USE_3DNOW,test "x$USE_3DNOW" = xyes)
AC_SUBST(USE_3DNOW_TRUE)
dnl ========================

AC_CACHE_CHECK(for socklen_t, xmms_cv_type_socklen_t,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>
#include <sys/socket.h>]], [[socklen_t s;]])],[xmms_cv_type_socklen_t=yes],[xmms_cv_type_socklen_t=no])])
if test "x$xmms_cv_type_socklen_t" = "xno"; then
  AC_DEFINE(socklen_t, int, [Define to int if the socklen_t type is missing])
fi


AC_MSG_CHECKING(for OSS include dir)
OSS_CFLAGS=""
if test -f "/etc/oss.conf"
then
	for i in `cat /etc/oss.conf`; 
	do
		t=`echo $i | sed -e 's/OSSLIBDIR=//'`
		if test "x$i" != "x$t"
		then
			if test -f "$t/include/sys/soundcard.h"
			then
				OSS_CFLAGS="-I$t/include"
			fi
		fi
	done
fi
if test -n "$OSS_CFLAGS"
then
	AC_MSG_RESULT("$OSS_CFLAGS")
else
	AC_MSG_RESULT("not found")
fi
AC_SUBST(OSS_CFLAGS)
CPPFLAGS_save=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $OSS_CFLAGS"
AC_CHECK_HEADERS(sys/soundcard.h)
AC_CHECK_HEADERS(machine/soundcard.h)
CPPFLAGS=$CPPFLAGS_save


dnl FreeBSD newpcm driver detection
AC_CACHE_CHECK(for FreeBSD newpcm driver, xmms_cv_newpcm_driver,
  if test -r "/dev/sndstat"; then
    grep 'newpcm' /dev/sndstat 2>/dev/null 1>/dev/null
    if test "x$?" = "x0"; then
      xmms_cv_newpcm_driver="yes"
    else
      xmms_cv_newpcm_driver="no"
    fi
  else
    xmms_cv_newpcm_driver="no"
  fi)
if test "x$xmms_cv_newpcm_driver" = "xyes"; then
  AC_DEFINE(HAVE_NEWPCM, 1, [Define if you have the FreeBSD newpcm driver])
fi


AC_CHECK_HEADERS(linux/cdrom.h)
AC_CHECK_HEADERS(sys/cdio.h)
AC_CHECK_HEADERS(linux/joystick.h)
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(fcntl.h)
AC_CHECK_HEADERS(sys/ioctl.h)
AC_CHECK_HEADERS(fnmatch.h)
AC_CHECK_HEADERS(limits.h)
AC_CHECK_HEADERS(sched.h)
AC_CHECK_HEADERS(sys/sched.h)
AC_CHECK_HEADERS(wchar.h)
AC_CHECK_HEADERS(fts.h)

XMMS_FUNC_POSIX(sched_setscheduler)

AC_CHECK_FUNCS(mkdtemp)
AC_CHECK_FUNCS(getmntinfo)
AC_CHECK_FUNCS(sched_get_priority_max)
AC_CHECK_FUNCS(srandomdev)
AC_CHECK_FUNCS(iconv_open)

AC_CACHE_CHECK([for nl_langinfo(CODESET)], xmms_cv_func_langinfo, [
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <langinfo.h>]], [[char *codeset = nl_langinfo (CODESET);]])],[xmms_cv_func_langinfo=yes],[xmms_cv_func_langinfo=no])])
if test "x$xmms_cv_func_langinfo" = "xyes"; then
  AC_DEFINE(HAVE_CODESET,,[Define if nl_langinfo(CODESET) is available.])
fi


AM_CONDITIONAL(HAVE_OSS,test "${ac_cv_header_sys_soundcard_h}" = "yes" || test "${ac_cv_header_machine_soundcard_h}" = "yes")
AM_CONDITIONAL(HAVE_CDROM, test "${ac_cv_header_linux_cdrom_h}" = "yes" || test "${ac_cv_header_sys_cdio_h}" = "yes")
AM_CONDITIONAL(HAVE_LINUX_JOYSTICK,test "${ac_cv_header_linux_joystick_h}" = "yes")

AC_CHECK_SIZEOF(long)

if test -n "$GCC"
then
  CFLAGS="$CFLAGS -Wall -Wpointer-arith -finline-functions -ffast-math"
  case "$CFLAGS" in
    -g*) ;;
    *\ -g*) ;;
    *) CFLAGS="$CFLAGS -fomit-frame-pointer" ;;
  esac
  case "$host" in
    *-*-freebsd*)
	;;
    *)
	CFLAGS="$CFLAGS -Wno-incompatible-pointer-types-discards-qualifiers -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-int-to-void-pointer-cast -Wno-deprecated-non-prototype"
	;;
  esac
fi

arch_type=other
have_solaris=no
ARCH_DEFINES=""
case "$host" in 
	*-*-openbsd* | *-*-netbsd* | *-*darwin*)
		AC_DEFINE(SYMBOL_PREFIX, "_", [Define to symbol prefix, if any])
	;;
	i386-*-* | i486-*-* | i586-*-* | i686-*-* | i86pc-*-*)
		ARCH_DEFINES="-DI386_ASSEM"
		arch_type=ix86
	;;
	ppc-*-*)
		arch_type=ppc
        ;;
        arm-*-*)
                arch_type=arm
	;;
	*-hpux-*)
		ARCH_DEFINES="-DHPUX"
	;;
	*-sun-*)
		have_solaris=yes
	;;
esac

AC_SUBST(ARCH_DEFINES)
AM_CONDITIONAL(ARCH_X86, test "x$arch_type" = "xix86")
AM_CONDITIONAL(ARCH_PPC, test "x$arch_type" = "xppc")
AM_CONDITIONAL(HAVE_SOLARIS, test "x$have_solaris" = xyes)

plugindir=$libdir/xmms
AC_SUBST(plugindir)

if test "x$enable_one_plugin_dir" = "xyes"; then
  pluginsubs="\\\"Plugins\\\""
  INPUT_PLUGIN_DIR=Plugins
  OUTPUT_PLUGIN_DIR=Plugins
  EFFECT_PLUGIN_DIR=Plugins
  GENERAL_PLUGIN_DIR=Plugins
  VISUALIZATION_PLUGIN_DIR=Plugins
else
  pluginsubs="\\\"Output\\\",\\\"Input\\\",\\\"Effect\\\",\\\"General\\\",\\\"Visualization\\\""
  INPUT_PLUGIN_DIR=Input
  OUTPUT_PLUGIN_DIR=Output
  EFFECT_PLUGIN_DIR=Effect
  GENERAL_PLUGIN_DIR=General
  VISUALIZATION_PLUGIN_DIR=Visualization
fi

AC_SUBST(INPUT_PLUGIN_DIR)
AC_SUBST(OUTPUT_PLUGIN_DIR)
AC_SUBST(EFFECT_PLUGIN_DIR)
AC_SUBST(GENERAL_PLUGIN_DIR)
AC_SUBST(VISUALIZATION_PLUGIN_DIR)
AC_SUBST(pluginsubs)

XMMS_DEFINES="$CFLAGS -DDATA_DIR=\\\"$datadir/xmms\\\" -DPLUGIN_DIR=\\\"$plugindir\\\" -DPLUGINSUBS=$pluginsubs -DLOCALEDIR=\\\"$localedir\\\""
AC_SUBST([XMMS_DEFINES])

ARCH_DEFINES="-DARCH_SPECIFIC_DEFINE"
AC_SUBST([ARCH_DEFINES])

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

# Output files to be generated
AC_CONFIG_FILES([Makefile po/Makefile.in xmms/Makefile])
AC_OUTPUT


