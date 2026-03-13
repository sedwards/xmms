lib_LTLIBRARIES = libcoreaudio.la

libcoreaudio_la_LDFLAGS = @PLUGIN_LDFLAGS@ -Wl,-framework,CoreAudio -Wl,-framework,AudioUnit -Wl,-framework,CoreServices
libcoreaudio_la_LIBADD = @GTK_LIBS@ @PTHREAD_LIBS@

libcoreaudio_la_SOURCES = core_audio.c

AM_CPPFLAGS = @GTK_CFLAGS@ -I$(top_srcdir)/xmms -I$(top_builddir)/intl -I$(top_srcdir)
