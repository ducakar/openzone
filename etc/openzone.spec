Name:           openzone
Version:        0.2.85
Release:        1
Summary:        Simple cross-platform FPS/RTS game (meta package)
URL:            https://github.com/ducakar/openzone/
License:        GPLv3+
Group:          Amusements/Games
Packager:       Davorin Učakar <davorin.ucakar@gmail.com>

Source:         openzone-src-%{version}.tar.xz
Source1:        openzone-data-ozbase-%{version}.tar.xz
Source2:        openzone-data-openzone-%{version}.tar.xz

Requires:       %{name}-client = %{version}
Requires:       %{name}-data = %{version}

%package -n liboz
Summary:        OpenZone core library
License:        MIT
Group:          System Environment/Libraries

%package -n liboz-devel
Summary:        Headers and documentation for OpenZone core library
License:        MIT
Group:          Development/Libraries
Requires:       liboz = %{version}

%package client
Summary:        Simple cross-platform FPS/RTS game engine

%package tools
Summary:        Tools for building game data for OpenZone engine
Group:          Development/Tools

%package data
Summary:        OpenZone game data
License:        Custom
Group:          Amusements/Games
Requires:       %{name}-client = %{version}

%package doc
Summary:        OpenZone engine documentation
Group:          Documentation

%description
OpenZone is a relatively simple cross-platform game engine, suitable for FPS,
RTS and RPG genres. It is strictly divided into several layers. Back-end runs
a complete simulation of a world (physics, object handlers, controllers, AI)
and front-end that renders it and enables the player to manipulate with the
simulated world.
This package only includes engine and essential data needed for engine to run.
Game data and missions packages must be installed separately.

%description -n liboz
Library provides facilities like container templates, array utilities,
string class, memory manager with memory leak tracing, crash handlers,
I/O buffers and classes for filesystem access with PhysicsFS support,
log writer, configuration file manipulation class, math functions and
linear algebra classes.
Library is primarily intended for use in OpenZone game engine.

%description -n liboz-devel
This package contains doxygen-generated documentation, header files and
pkg-config configuration needed for develipment using liboz.

%description client
OpenZone is a relatively simple cross-platform game engine, suitable for FPS,
RTS and RPG genres. It is strictly divided into several layers. Back-end runs
a complete simulation of a world (physics, object handlers, controllers, AI)
and front-end that renders it and enables the player to manipulate with the
simulated world.
This package only includes engine and essential data needed for engine to run.
Game data and missions packages must be installed separately.

%description tools
ozBase tool for building game data packages for OpenZone engine and some other
miscellaneous tools.

%description data
Game data for OpenZone. Includes tutorials, testing world and cviček mission.

%description doc
Doxygen-generated documentation for OpenZone engine and PDF articles describing
concepts and algorithms used in the engine.

%prep
%setup -q -b 1 -b 2 -n openzone

%build
mkdir -p build && cd build

cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make %{?_smp_mflags} doc
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
cd build

make install DESTDIR=$RPM_BUILD_ROOT

install -Dm644 %{_builddir}/ozbase.zip $RPM_BUILD_ROOT/%{_datadir}/openzone/ozbase.zip
install -Dm644 %{_builddir}/openzone.zip $RPM_BUILD_ROOT/%{_datadir}/openzone/openzone.zip

%files
%defattr(-, root, root)

%files -n liboz
%defattr(-, root, root)
%{_libdir}/liboz.so*
%doc AUTHORS src/oz/COPYING

%files -n liboz-devel
%defattr(-, root, root)
%{_includedir}/oz
%{_libdir}/pkgconfig
%doc AUTHORS src/oz/COPYING
%doc doc/doxygen.liboz/html

%files client
%defattr(-, root, root)
%{_bindir}/openzone
%{_datadir}/applications
%{_datadir}/pixmaps
%dir %{_datadir}/openzone
%{_datadir}/openzone/ozbase.zip
%doc AUTHORS COPYING README* ChangeLog BUGS TODO

%files tools
%defattr(-, root, root)
%{_bindir}/ozBuild
%{_bindir}/ozFormat
%doc AUTHORS COPYING

%files data
%defattr(-, root, root)
%dir %{_datadir}/openzone
%{_datadir}/openzone/openzone.zip

%files doc
%defattr(-, root, root)
%doc doc/doxygen/html
