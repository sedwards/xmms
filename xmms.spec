%define	name	xmms
%define	version	1.2.6
%define	release	1
%define	serial	1
%define	prefix	/usr

## Check to see if libGL is installed. Build xmms-gl if it is.
%define withGL	%(if [ -z "`rpm -q --whatprovides libGL.so.1 2>/dev/null`" ]; then echo 0; else echo 1; fi)

## Check to see if libmikmod is installed. Build xmms-mikmod if it is.
%define withmm	%(if [ -z "`rpm -q --whatprovides libmikmod.so.2 2>/dev/null`" ]; then echo 0; else echo 1; fi)
%define wmmdev	%(if [ -z "`rpm -q --whatprovides $(/usr/bin/which libmikmod-config) 2>/dev/null`" ]; then echo 0; else echo 1; fi)

## Check to see if libvorbisfile is installed.  Build xmms-vorbis if it is.
%define withvorbis %(if [ -z "`rpm -q --whatprovides libvorbisfile.so.0 2>/dev/null`" ]; then echo 0; else echo 1; fi)

## Funky hack to get package names that provide libmikmod and libmikmod-config
## Becuase of the differing package names between redhat, mandrake, etc.
%if %{withmm} == 1
%define mikmod	%(rpm -q --qf '%{NAME}' --whatprovides libmikmod.so.2)
%if %{wmmdev} == 1
%define mmdev	%(rpm -q --qf '%{NAME}' --whatprovides $(/usr/bin/which libmikmod-config))
%endif
%endif

%define gnomedatadir %(gnome-config --datadir 2> /dev/null || echo "/usr/share")
%define gnomesysconfdir %(gnome-config --sysconfdir 2> /dev/null || echo "/etc")

Summary:	XMMS - Multimedia player for the X Window System.
Name:		%{name}
Version:	%{version}
Release:	%{release}
Serial:		%{serial}
Copyright:	GPL
Group:		Applications/Multimedia
Vendor:		XMMS Development Team <bugs@xmms.org>
Url:		http://www.xmms.org/
Source:		%{name}-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-root
Obsoletes:	x11amp, x11amp0.7-1-1
Requires:	gtk+ >= 1.2.2

%description
X MultiMedia System is a sound player written from scratch. Since it 
uses the WinAmp GUI, it can use WinAmp skins. It can play mp3s, mods, s3ms,
and other formats. It now has support for input, output, general, and
visualization plugins.

%package	devel
Summary:	XMMS - Static libraries and header files.
Group:		Applications/Multimedia
Obsoletes:	x11amp-devel
Requires:	%{name} = %{version}, glib-devel >= 1.2.2, gtk+-devel >= 1.2.2

%description	devel
Static libraries and header files required for compiling xmms plugins.

%package	esd
Summary:	XMMS - Output plugin for use with the esound package.
Group:		Applications/Multimedia
Requires:	%{name} = %{version}
Obsoletes:	x11amp-esd
Requires:	esound >= 0.2.8

%description	esd
Output plugin for xmms for use with the esound package

%package	gnome
Summary:	XMMS - applet for controlling xmms from the GNOME panel.
Group:		Applications/Multimedia
Requires:	%{name} = %{version}
Obsoletes:	x11amp-gnome
Requires:	gnome-libs >= 1.0.0

%description	gnome
GNOME applet for controlling xmms from the GNOME panel

%if %{withmm} == 1
%package	mikmod
Summary:	XMMS - Input plugin to play MODs.
Group:		Applications/Multimedia
Obsoletes:	x11amp-mikmod
Requires:	%{name} = %{version}
Requires:	%{mikmod} >= 3.1.6
%if %{wmmdev} == 1
BuildPrereq:	%{mmdev}
%else
BuildPrereq:	/usr/bin/libmikmod-config
%endif

%description	mikmod
Input plugin for XMMS to play MODs (.MOD,.XM,.S3M, etc)
%endif

%if %{withvorbis} == 1
%package	vorbis
Summary:	XMMS - Input plugin to play OGGs
Group:		Applications/Multimedia
Requires:	%{name} = %{version}
Requires:	libogg >= 1.0beta3
Requires:	libvorbis >= 1.0beta3
BuildPrereq:	libogg-devel
BuildPrereq:	libvorbis-devel

%description	vorbis
Input plugin for XMMS to play Ogg Vorbis files (.ogg).
%endif

%if %{withGL} == 1
%package 	gl
Summary:	XMMS - Visualization plugins that use the Mesa3d library.
Group:		Applications/Multimedia
Requires:	%{name} = %{version}
Obsoletes:	xmms-mesa

%description	gl
Visualization plugins that use the Mesa3d library.
%endif

%prep
%setup -q

%build
export CFLAGS="${RPM_OPT_FLAGS}" CPPFLAGS="${RPM_OPT_FLAGS}" CXXFLAGS="${RPM_OPT_FLAGS}";
unset LINGUAS || :;

./configure --prefix=%{prefix};
make

%install
[ "${RPM_BUILD_ROOT}" != "/" ] && [ -d ${RPM_BUILD_ROOT} ] && rm -rf ${RPM_BUILD_ROOT};
mkdir -p ${RPM_BUILD_ROOT}
make prefix=${RPM_BUILD_ROOT}%{prefix} \
	sysconfdir=${RPM_BUILD_ROOT}/etc \
	GNOME_DATADIR=${RPM_BUILD_ROOT}%{gnomedatadir} \
	GNOME_SYSCONFDIR=${RPM_BUILD_ROOT}%{gnomesysconfdir} install

# Strip binaries. But, not plugins.
strip --strip-unneeded ${RPM_BUILD_ROOT}%{prefix}/bin/*xmms;

# Install icons.
mkdir -p ${RPM_BUILD_ROOT}/usr/share/pixmaps/mini
install -m 644 xmms/xmms_logo.xpm \
	${RPM_BUILD_ROOT}/usr/share/pixmaps/xmms_logo.xpm
install -m 644 xmms/xmms_mini.xpm \
	${RPM_BUILD_ROOT}/usr/share/pixmaps/mini/xmms_mini.xpm

# Install wmconfig file
mkdir -p ${RPM_BUILD_ROOT}/etc/X11/wmconfig
install -m 644 xmms/xmms.wmconfig \
	${RPM_BUILD_ROOT}/etc/X11/wmconfig/xmms

# Install applnk file
mkdir -p ${RPM_BUILD_ROOT}/etc/X11/applnk/Multimedia/                             
install -m 644 xmms/xmms.desktop \
	${RPM_BUILD_ROOT}/etc/X11/applnk/Multimedia/ 

%post	-p /sbin/ldconfig
%postun -p /sbin/ldconfig

%clean
[ "${RPM_BUILD_ROOT}" != "/" ] && [ -d ${RPM_BUILD_ROOT} ] && rm -rf ${RPM_BUILD_ROOT};

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README
/etc/X11/wmconfig/xmms
/etc/X11/applnk/Multimedia/xmms.desktop
%{prefix}/bin/xmms
%{prefix}/bin/wmxmms
%{prefix}/lib/libxmms.so.*
%{prefix}/lib/xmms/Input/libcdaudio*
%{prefix}/lib/xmms/Input/libidcin*
%{prefix}/lib/xmms/Input/libmpg123*
%{prefix}/lib/xmms/Input/libtonegen*
%{prefix}/lib/xmms/Input/libwav*
%{prefix}/lib/xmms/Output/libOSS*
%{prefix}/lib/xmms/Output/libdisk_writer*
%{prefix}/lib/xmms/General/*
%{prefix}/lib/xmms/Effect/*
%{prefix}/lib/xmms/Visualization/libbscope*
%{prefix}/lib/xmms/Visualization/libsanalyzer*
%{prefix}/share/xmms/*
%{prefix}/man/man1/*
/usr/share/locale/*/LC_MESSAGES/xmms.mo
/usr/share/pixmaps/xmms_logo.xpm
/usr/share/pixmaps/mini/xmms_mini.xpm

%files devel
%defattr(-, root, root)
%{prefix}/bin/xmms-config
%{prefix}/lib/lib*.so
%{prefix}/lib/lib*.a
%{prefix}/include/*
%{prefix}/share/aclocal/xmms.m4

%files esd
%defattr(-, root, root)
%{prefix}/lib/xmms/Output/libesdout*

%files gnome
%defattr(-, root, root)
%{prefix}/bin/gnomexmms
%{gnomesysconfdir}/CORBA/servers/gnomexmms.gnorba
%{gnomedatadir}/applets/Multimedia/gnomexmms.desktop

%if %{withmm} == 1
%files mikmod
%defattr(-, root, root)
%{prefix}/lib/xmms/Input/libmikmod*
%endif

%if %{withvorbis} == 1
%files vorbis
%defattr(-, root, root)
%{prefix}/lib/xmms/Input/libvorbis*
%endif

%if %{withGL} == 1
%files gl
%defattr(-, root, root)
%{prefix}/lib/xmms/Visualization/libogl_spectrum*
%endif

%changelog

