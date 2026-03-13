lib_LTLIBRARIES = libwav.la

libdir = $(plugindir)/$(INPUT_PLUGIN_DIR)

libwav_la_LDFLAGS = @PLUGIN_LDFLAGS@

libwav_la_SOURCES = 	\
wav.c \
wav.h

AM_CPPFLAGS = @GTK_CFLAGS@ -I$(top_srcdir)/intl -I$(top_srcdir)/xmms
