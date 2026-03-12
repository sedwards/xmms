AUTOMAKE_OPTIONS = foreign
xmmsincludedir = $(includedir)/xmms

lib_LTLIBRARIES = libxmms.la

libxmms_la_LDFLAGS = -export-dynamic -version-info $(LIBXMMS_MAJOR_VERSION):$(LIBXMMS_MINOR_VERSION):$(LIBXMMS_MICRO_VERSION)

if HAVE_MACOS
libxmms_la_LDFLAGS += -framework Cocoa
endif

libxmms_la_LIBADD = @GTK_LIBS@ 
AM_CPPFLAGS = @GTK_CFLAGS@ -I../intl \
    -I$(top_srcdir)/xmms \
    -I$(top_builddir)/xmms

libxmms_la_SOURCES = configfile.c configfile.h xmmsctrl.c xmmsctrl.h dirbrowser.h util.c util.h formatter.c formatter.h titlestring.c titlestring.h darwin_dirbrowser.m

if HAVE_MACOS
libxmms_la_SOURCES += \
    darwin_dirbrowser.m \
    audio_coreaudio.m \
    metal_visualizer.m
else
libxmms_la_SOURCES += \
    dirbrowser.c
endif

xmmsinclude_HEADERS = configfile.h xmmsctrl.h dirbrowser.h util.h formatter.h titlestring.h

