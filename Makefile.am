ACLOCAL_AMFLAGS = -I m4

#xmms_LDADD = $(GTK_LIBS)

xmms_LDADD = $(GTK_LIBS) -framework Cocoa

SUBDIRS = libxmms xmms Output Input Effect General Visualization

bin_SCRIPTS = xmms-config

EXTRA_DIST = xmms.spec xmms.spec.in FAQ xmms.m4

m4datadir = $(datadir)/aclocal
m4data_DATA = xmms.m4

man_MANS =

install-exec-hook:
	$(LIBTOOL) --finish $(DESTDIR)$(plugindir)/Input
	$(LIBTOOL) --finish $(DESTDIR)$(plugindir)/Output
	$(LIBTOOL) --finish $(DESTDIR)$(plugindir)/Effect
	$(LIBTOOL) --finish $(DESTDIR)$(plugindir)/General
	$(LIBTOOL) --finish $(DESTDIR)$(plugindir)/Visualization
