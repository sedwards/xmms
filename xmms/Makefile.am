SUBDIRS = defskin

xmmsincludedir = $(includedir)/xmms

bin_PROGRAMS = xmms

xmms_LDFLAGS = -export-dynamic
xmms_LDADD = $(GTK_LIBS)  @PTHREAD_LIBS@ @SM_LIBS@ @VM_LIBS@ \
@POSIX_LIBS@ $(top_builddir)/libxmms/libxmms.la @INTLLIBS@

INCLUDES = @GTK_CFLAGS@ @XMMS_DEFINES@ -I$(top_builddir)/intl

xmms_SOURCES =  \
main.c main.h \
bmp.c bmp.h \
fft.c fft.h \
getopt.c getopt1.c getopt.h \
sm.c sm.h \
urldecode.c urldecode.h \
mkdtemp.c

EXTRA_DIST= xmms_logo.xpm xmms_mini.xpm xmms.desktop xmms.wmconfig

xmmsinclude_HEADERS = plugin.h fullscreen.h
