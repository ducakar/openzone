Name:           openzone
Version:        0.3.81
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
Summary:        OpenZone Core and Dynamics libraries
License:        zlib
Group:          System Environment/Libraries

%package -n liboz-devel
Summary:        Headers for OpenZone Core and Dynamics libraries
License:        zlib
Group:          Development/Libraries
Requires:       liboz-core = %{version}

%package client
Summary:        Simple cross-platform FPS/RTS game engine

%package data
Summary:        OpenZone game data
License:        Custom
Group:          Amusements/Games
Requires:       %{name}-client = %{version}

%description
OpenZone is a relatively simple cross-platform game engine, suitable for FPS,
RTS and RPG genres. It is strictly divided into several layers. Back-end runs
a complete simulation of a world (physics, object handlers, controllers, AI)
and front-end that renders it and enables the player to manipulate with the
simulated world.
This is a meta package that installs engine and game data.

%description -n liboz
OpenZone Core Library provides facilities like container templates, array
utilities, string class, memory manager with memory leak tracing, crash
handlers, I/O buffers and classes for filesystem access with PhysicsFS support,
log writer, JSON file manipulation class, math functions and linear algebra
classes.
OpenZone Dynamics Library is a simple rigid body physics engine used in OpenZone
game.

%description -n liboz-devel
This package contains header files OpenZone Core and Dynamics libraries.

%description client
OpenZone is a relatively simple cross-platform game engine, suitable for FPS,
RTS and RPG genres. It is strictly divided into several layers. Back-end runs
a complete simulation of a world (physics, object handlers, controllers, AI)
and front-end that renders it and enables the player to manipulate with the
simulated world.
This package only includes engine. Game data packages must be installed
separately.

%description data
Game data for OpenZone. Includes tutorial, testing world and cviček missions.

%prep
%setup -q -b 1

%build
cd build

cmake \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_INSTALL_PREFIX=/usr \
  -D CMAKE_CXX_FLAGS="-msse3 -mfpmath=sse" \
  ..

make %{?_smp_mflags}

%install
rm -rf "$RPM_BUILD_ROOT"

install -dm755 "$RPM_BUILD_ROOT"%{_datadir}/openzone
install -m644 share/openzone/*.zip "$RPM_BUILD_ROOT"%{_datadir}/openzone

cd build

make install DESTDIR="$RPM_BUILD_ROOT"
rm -rf "$RPM_BUILD_ROOT"%{_defaultdocdir}

if [[ %{_libdir} != /usr/lib ]]; then
  sed -ri 's|libdir=.*|libdir=%{_libdir}|' "$RPM_BUILD_ROOT"/usr/lib/pkgconfig/*
  mv "$RPM_BUILD_ROOT"/usr/lib "$RPM_BUILD_ROOT"%{_libdir}
fi

%files
%defattr(-, root, root)

%files -n liboz
%defattr(-, root, root)
%{_libdir}/libozCore.so*
%{_libdir}/libozDynamics.so*
%doc src/ozCore/COPYING

%files -n liboz-devel
%defattr(-, root, root)
%{_libdir}/pkgconfig/ozCore.pc
%{_libdir}/pkgconfig/ozDynamics.pc
%{_includedir}/ozCore
%{_includedir}/ozDynamics
%doc src/ozCore/COPYING

%files client
%defattr(-, root, root)
%{_bindir}/*
%{_datadir}/applications
%{_datadir}/pixmaps
%doc AUTHORS COPYING README.md ChangeLog.md doc/*.html doc/licences

%files data
%defattr(-, root, root)
%{_datadir}/openzone
