# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
#
# This file file be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

PACKAGE = xmms
VERSION = 0.0.1

SHELL = /bin/sh


srcdir = .
top_srcdir = ..


prefix = /usr/local
exec_prefix = ${prefix}
datadir = $(prefix)/@DATADIRNAME@
localedir = $(datadir)/locale
gnulocaledir = $(prefix)/share/locale
gettextsrcdir = $(prefix)/share/gettext/po
subdir = po

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = $(top_srcdir)/@MKINSTALLDIRS@

CC = gcc
GENCAT = @GENCAT@
GMSGFMT = PATH=../src:$$PATH /opt/local/bin/msgfmt
MSGFMT = /opt/local/bin/msgfmt
XGETTEXT = PATH=../src:$$PATH /opt/local/bin/xgettext
MSGMERGE = PATH=../src:$$PATH msgmerge

DEFS = -DHAVE_CONFIG_H
CFLAGS = -L/opt/homebrew/lib  -Wall -Wpointer-arith -finline-functions -ffast-math -fomit-frame-pointer -funroll-all-loops
CPPFLAGS = -I/opt/homebrew/opt/ruby/include

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

SOURCES = cat-id-tbl.c
POFILES = @POFILES@
GMOFILES = @GMOFILES@
DISTFILES = ChangeLog Makefile.in.in POTFILES.in $(PACKAGE).pot \
stamp-cat-id $(POFILES) $(GMOFILES) $(SOURCES)

POTFILES = \
     ../Effect/echo_plugin/echo.c \
     ../Effect/echo_plugin/gui.c \
     ../Effect/stereo_plugin/stereo.c \
     ../Effect/voice/about.c \
     ../Effect/voice/voice.c \
     ../General/ir/about.c \
     ../General/ir/configure.c \
     ../General/ir/ir.c \
     ../General/ir/irfunc.c \
     ../General/ir/irio.c \
     ../General/joystick/about.c \
     ../General/joystick/configure.c \
     ../General/joystick/joy.c \
     ../General/song_change/song_change.c \
     ../Input/cdaudio/cdaudio.c \
     ../Input/cdaudio/cddb.c \
     ../Input/cdaudio/cdindex.c \
     ../Input/cdaudio/cdinfo.c \
     ../Input/cdaudio/configure.c \
     ../Input/cdaudio/http.c \
     ../Input/cdaudio/sha_func.c \
     ../Input/idcin/idcin.c \
     ../Input/mikmod/drv_xmms.c \
     ../Input/mikmod/plugin.c \
     ../Input/mpg123/common.c \
     ../Input/mpg123/configure.c \
     ../Input/mpg123/dct64.c \
     ../Input/mpg123/dct64_i386.c \
     ../Input/mpg123/decode.c \
     ../Input/mpg123/decode_2to1.c \
     ../Input/mpg123/decode_4to1.c \
     ../Input/mpg123/decode_i386.c \
     ../Input/mpg123/decode_ntom.c \
     ../Input/mpg123/dxhead.c \
     ../Input/mpg123/equalizer.c \
     ../Input/mpg123/fileinfo.c \
     ../Input/mpg123/http.c \
     ../Input/mpg123/id3.c \
     ../Input/mpg123/id3_frame.c \
     ../Input/mpg123/id3_frame_content.c \
     ../Input/mpg123/id3_frame_text.c \
     ../Input/mpg123/id3_frame_url.c \
     ../Input/mpg123/id3_tag.c \
     ../Input/mpg123/layer1.c \
     ../Input/mpg123/layer2.c \
     ../Input/mpg123/layer3.c \
     ../Input/mpg123/mpg123.c \
     ../Input/mpg123/tabinit.c \
     ../Input/tonegen/tonegen.c \
     ../Input/vorbis/configure.c \
     ../Input/vorbis/fileinfo.c \
     ../Input/vorbis/http.c \
     ../Input/vorbis/vorbis.c \
     ../Input/wav/wav.c \
     ../Output/OSS/OSS.c \
     ../Output/OSS/about.c \
     ../Output/OSS/audio.c \
     ../Output/OSS/configure.c \
     ../Output/OSS/init.c \
     ../Output/OSS/mixer.c \
     ../Output/disk_writer/disk_writer.c \
     ../Output/esd/about.c \
     ../Output/esd/audio.c \
     ../Output/esd/configure.c \
     ../Output/esd/esd.c \
     ../Output/esd/init.c \
     ../Output/esd/mixer.c \
     ../Output/solaris/Sun.c \
     ../Output/solaris/about.c \
     ../Output/solaris/configure.c \
     ../Visualization/blur_scope/blur_scope.c \
     ../Visualization/blur_scope/config.c \
     ../Visualization/opengl_spectrum/configure.c \
     ../Visualization/opengl_spectrum/opengl_spectrum.c \
     ../Visualization/sanalyzer/spectrum.c \
     ../gnomexmms/gnomexmms.c \
     ../libxmms/configfile.c \
     ../libxmms/dirbrowser.c \
     ../libxmms/titlestring.c \
     ../libxmms/util.c \
     ../libxmms/xmmsctrl.c \
     ../wmxmms/getopt.c \
     ../wmxmms/getopt1.c \
     ../wmxmms/wmxmms.c \
     ../xmms/about.c \
     ../xmms/bmp.c \
     ../xmms/controlsocket.c \
     ../xmms/dock.c \
     ../xmms/effect.c \
     ../xmms/eq_graph.c \
     ../xmms/eq_slider.c \
     ../xmms/equalizer.c \
     ../xmms/fft.c \
     ../xmms/fullscreen.c \
     ../xmms/general.c \
     ../xmms/getopt.c \
     ../xmms/getopt1.c \
     ../xmms/hints.c \
     ../xmms/hslider.c \
     ../xmms/input.c \
     ../xmms/main.c \
     ../xmms/menurow.c \
     ../xmms/monostereo.c \
     ../xmms/number.c \
     ../xmms/output.c \
     ../xmms/pbutton.c \
     ../xmms/playlist.c \
     ../xmms/playlist_list.c \
     ../xmms/playlist_popup.c \
     ../xmms/playlist_slider.c \
     ../xmms/playlistwin.c \
     ../xmms/playstatus.c \
     ../xmms/pluginenum.c \
     ../xmms/prefswin.c \
     ../xmms/sbutton.c \
     ../xmms/skin.c \
     ../xmms/skinwin.c \
     ../xmms/sm.c \
     ../xmms/svis.c \
     ../xmms/tbutton.c \
     ../xmms/textbox.c \
     ../xmms/util.c \
     ../xmms/vis.c \
     ../xmms/visualization.c \
     ../xmms/widget.c

export OLD_PO_FILE_INPUT = yes

CATALOGS = @CATALOGS@
CATOBJEXT = @CATOBJEXT@
INSTOBJEXT = @INSTOBJEXT@

.SUFFIXES:
.SUFFIXES: .c .o .po .pox .gmo .mo .msg .cat

.c.o:
	$(COMPILE) $<

.po.pox:
	$(MAKE) $(PACKAGE).pot
	$(MSGMERGE) $< $(srcdir)/$(PACKAGE).pot -o $*.pox

.po.mo:
	$(MSGFMT) -o $@ $<

.po.gmo:
	file=$(srcdir)/`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) -o $$file $<

.po.cat:
	sed -f ../intl/po2msg.sed < $< > $*.msg \
	  && rm -f $@ && $(GENCAT) $@ $*.msg


all: all-no

all-yes: cat-id-tbl.c $(CATALOGS)
all-no:

$(srcdir)/$(PACKAGE).pot: $(POTFILES)
	$(XGETTEXT) --default-domain=$(PACKAGE) --directory=$(top_srcdir) \
	  --add-comments --keyword=_ --keyword=N_ \
	  --files-from=$(srcdir)/POTFILES.in \
	&& test ! -f $(PACKAGE).po \
	   || ( rm -f $(srcdir)/$(PACKAGE).pot \
		&& mv $(PACKAGE).po $(srcdir)/$(PACKAGE).pot )

$(srcdir)/cat-id-tbl.c: stamp-cat-id; @:
$(srcdir)/stamp-cat-id: $(PACKAGE).pot
	rm -f cat-id-tbl.tmp
	sed -f ../intl/po2tbl.sed $(srcdir)/$(PACKAGE).pot \
		| sed -e "s/@PACKAGE NAME@/$(PACKAGE)/" > cat-id-tbl.tmp
	if cmp -s cat-id-tbl.tmp $(srcdir)/cat-id-tbl.c; then \
	  rm cat-id-tbl.tmp; \
	else \
	  echo cat-id-tbl.c changed; \
	  rm -f $(srcdir)/cat-id-tbl.c; \
	  mv cat-id-tbl.tmp $(srcdir)/cat-id-tbl.c; \
	fi
	cd $(srcdir) && rm -f stamp-cat-id && echo timestamp > stamp-cat-id


install: install-exec install-data
install-exec:
install-data: install-data-no
install-data-no: all
install-data-yes: all
	if test -r "$(MKINSTALLDIRS)"; then \
	  $(MKINSTALLDIRS) $(datadir); \
	else \
	  $(SHELL) $(top_srcdir)/mkinstalldirs $(datadir); \
	fi
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  case "$$cat" in \
	    *.gmo) destdir=$(gnulocaledir);; \
	    *)     destdir=$(localedir);; \
	  esac; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  dir=$$destdir/$$lang/LC_MESSAGES; \
	  if test -r "$(MKINSTALLDIRS)"; then \
	    $(MKINSTALLDIRS) $$dir; \
	  else \
	    $(SHELL) $(top_srcdir)/mkinstalldirs $$dir; \
	  fi; \
	  if test -r $$cat; then \
	    $(INSTALL_DATA) $$cat $$dir/$(PACKAGE)$(INSTOBJEXT); \
	    echo "installing $$cat as $$dir/$(PACKAGE)$(INSTOBJEXT)"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$cat $$dir/$(PACKAGE)$(INSTOBJEXT); \
	    echo "installing $(srcdir)/$$cat as" \
		 "$$dir/$(PACKAGE)$(INSTOBJEXT)"; \
	  fi; \
	  if test -r $$cat.m; then \
	    $(INSTALL_DATA) $$cat.m $$dir/$(PACKAGE)$(INSTOBJEXT).m; \
	    echo "installing $$cat.m as $$dir/$(PACKAGE)$(INSTOBJEXT).m"; \
	  else \
	    if test -r $(srcdir)/$$cat.m ; then \
	      $(INSTALL_DATA) $(srcdir)/$$cat.m \
		$$dir/$(PACKAGE)$(INSTOBJEXT).m; \
	      echo "installing $(srcdir)/$$cat as" \
		   "$$dir/$(PACKAGE)$(INSTOBJEXT).m"; \
	    else \
	      true; \
	    fi; \
	  fi; \
	done
	if test "$(PACKAGE)" = "gettext"; then \
	  if test -r "$(MKINSTALLDIRS)"; then \
	    $(MKINSTALLDIRS) $(gettextsrcdir); \
	  else \
	    $(SHELL) $(top_srcdir)/mkinstalldirs $(gettextsrcdir); \
	  fi; \
	  $(INSTALL_DATA) $(srcdir)/Makefile.in.in \
			  $(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi

# Define this as empty until I found a useful application.
installcheck:

uninstall:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  rm -f $(localedir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT); \
	  rm -f $(localedir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT).m; \
	  rm -f $(gnulocaledir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT); \
	  rm -f $(gnulocaledir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT).m; \
	done
	rm -f $(gettextsrcdir)/po-Makefile.in.in

check: all

cat-id-tbl.o: ../intl/libgettext.h

dvi info tags TAGS ID:

mostlyclean:
	rm -f core core.* *.pox $(PACKAGE).po *.old.po cat-id-tbl.tmp
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in POTFILES *.mo *.msg *.cat *.cat.m

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES)

distdir = ../$(PACKAGE)-$(VERSION)/$(subdir)
dist distdir: update-po $(DISTFILES)
	dists="$(DISTFILES)"; \
	for file in $$dists; do \
	  ln $(srcdir)/$$file $(distdir) 2> /dev/null \
	    || cp -p $(srcdir)/$$file $(distdir); \
	done

update-po: Makefile
	$(MAKE) $(PACKAGE).pot
	PATH=`pwd`/../src:$$PATH; \
	cd $(srcdir); \
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  mv $$lang.po $$lang.old.po; \
	  echo "$$lang:"; \
	  if $(MSGMERGE) $$lang.old.po $(PACKAGE).pot -o $$lang.po; then \
	    rm -f $$lang.old.po; \
	  else \
	    echo "msgmerge for $$cat failed!"; \
	    rm -f $$lang.po; \
	    mv $$lang.old.po $$lang.po; \
	  fi; \
	done

POTFILES: POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' -e '/^[ 	]*$$/d' \
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in \
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

Makefile: Makefile.in.in ../config.status POTFILES
	cd .. \
	  && CONFIG_FILES=$(subdir)/$@.in CONFIG_HEADERS= \
	       $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
