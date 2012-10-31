Name:           openzone
Version:        0.3.80
Release:        1
Summary:        Simple cross-platform FPS/RTS game engine (meta package)
URL:            http://ducakar.github.com/openzone/
License:        GPLv3+
Group:          Amusements/Games
Packager:       Davorin Učakar <davorin.ucakar@gmail.com>

Source:         openzone-src-%{version}.tar.xz
Source1:        openzone-data-%{version}.tar.xz

Requires:       %{name}-client = %{version}
Requires:       %{name}-data = %{version}

%package -n liboz
Summary:        OpenZone Core Library
License:        zlib
Group:          System Environment/Libraries

%package -n liboz-devel
Summary:        Headers and documentation for OpenZone core library
License:        zlib
Group:          Development/Libraries
Requires:       liboz = %{version}

%package -n libozdynamics
Summary:        OpenZone Dynamics Engine
License:        zlib
Group:          System Environment/Libraries
Requires:       liboz = %{version}

%package -n libozdynamics-devel
Summary:        Headers and documentation for OpenZone dynamics engine
License:        zlib
Group:          Development/Libraries
Requires:       libozde = %{version}, liboz-devel = %{version}

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
This is a meta package that installs engine and game data.

%description -n liboz
Library provides facilities like container templates, array utilities,
string class, memory manager with memory leak tracing, crash handlers,
I/O buffers and classes for filesystem access with PhysicsFS support,
log writer, configuration file manipulation class, math functions and
linear algebra classes.
Library is primarily intended for use in OpenZone game engine.

%description -n liboz-devel
This package contains doxygen-generated documentation, header files and
pkg-config configuration needed for development using liboz.

%description -n libozdynamics
OpenZone Dynamics Engine.

%description -n libozdynamics-devel
This package contains doxygen-generated documentation, header files and
pkg-config configuration needed for development using OpenZone Dynamics Engine.

%description client
OpenZone is a relatively simple cross-platform game engine, suitable for FPS,
RTS and RPG genres. It is strictly divided into several layers. Back-end runs
a complete simulation of a world (physics, object handlers, controllers, AI)
and front-end that renders it and enables the player to manipulate with the
simulated world.
This package only includes engine. Game data packages must be installed
separately.

%description tools
ozBase tool for building game data packages for OpenZone engine and some other
miscellaneous tools.

%description data
Game data for OpenZone. Includes tutorial, testing world and cviček missions.

%description doc
Doxygen-generated documentation for OpenZone engine and PDF articles describing
concepts and algorithms used in the engine.

%prep
%setup -q -b 1

%build
rm -rf build && mkdir build && cd build

cmake \
  -D CMAKE_BUILD_TYPE="Release" \
  -D CMAKE_INSTALL_PREFIX="/usr" \
  -D CMAKE_CXX_FLAGS="-msse3 -mfpmath=sse" \
  -D OZ_SHARED_LIBS="1" \
  -D OZ_NONFREE="1" \
  ..

make %{?_smp_mflags} doc
make %{?_smp_mflags}

%install
rm -rf "$RPM_BUILD_ROOT"

install -dm755 "$RPM_BUILD_ROOT"%{_datadir}/openzone
install -m644 share/openzone/*.zip "$RPM_BUILD_ROOT"%{_datadir}/openzone

cd build

make install DESTDIR="$RPM_BUILD_ROOT"

if [[ %{_libdir} != /usr/lib ]]; then
  mv "$RPM_BUILD_ROOT"/usr/lib "$RPM_BUILD_ROOT"%{_libdir}
fi

%files
%defattr(-, root, root)

%files -n liboz
%defattr(-, root, root)
%{_libdir}/liboz.so*
%doc AUTHORS src/oz/COPYING

%files -n liboz-devel
%defattr(-, root, root)
%{_includedir}/oz
%doc AUTHORS src/oz/COPYING doc/doxygen.liboz/html

%files -n libozdynamics
%defattr(-, root, root)
%{_libdir}/libozdynamics.so*
%doc AUTHORS src/ozdynamics/COPYING

%files -n libozdynamics-devel
%defattr(-, root, root)
%{_includedir}/ozdynamics
%doc AUTHORS src/ozdynamics/COPYING doc/doxygen.libozdynamics/html

%files client
%defattr(-, root, root)
%{_bindir}/openzone
%{_datadir}/applications
%{_datadir}/pixmaps
%doc AUTHORS COPYING README.md ChangeLog.md doc/*.html

%files tools
%defattr(-, root, root)
%{_bindir}/ozBuild
%{_bindir}/ozGettext
%{_bindir}/ozManifest
%doc AUTHORS COPYING README.md ChangeLog.md

%files data
%defattr(-, root, root)
%{_datadir}/openzone

%files doc
%defattr(-, root, root)
%doc doc/doxygen/html
