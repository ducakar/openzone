Name:           openzone
Version:        0.3.95
Release:        1%{?dist}
Summary:        Simple cross-platform FPS/RTS game engine
Group:          Amusements/Games
License:        GPLv3+
URL:            http://ducakar.github.com/openzone/
Packager:       Davorin Učakar <davorin.ucakar@gmail.com>
Requires:       %{name}-data = %{version}

Source0:        https://github.com/downloads/ducakar/openzone/%{name}-src-%{version}.tar.xz
Source1:        https://github.com/downloads/ducakar/openzone/%{name}-data-%{version}.tar.xz

BuildRequires:  cmake
BuildRequires:  alsa-lib-devel
BuildRequires:  zlib-devel
BuildRequires:  physfs-devel
BuildRequires:  lua-devel
BuildRequires:  SDL2-devel
BuildRequires:  SDL2_ttf-devel
BuildRequires:  mesa-libGL-devel
BuildRequires:  openal-soft-devel
BuildRequires:  libjpeg-devel
BuildRequires:  libvorbis-devel
BuildRequires:  freeimage-devel
BuildRequires:  libsquish-devel
BuildRequires:  assimp-devel

%description
OpenZone is a relatively simple cross-platform game engine, suitable for FPS,
RTS and RPG genres. It is strictly divided into several layers. Back-end runs
a complete simulation of a world (physics, object handlers, controllers, AI)
and front-end that renders it and enables the player to manipulate with the
simulated world.

%package -n liboz
Summary:        OpenZone ozCore, ozEngine and ozFactory libraries
Group:          System Environment/Libraries
License:        zlib

%description -n liboz
ozCore library provides facilities like container templates, array utilities,
string class, memory manager with memory leak tracing, crash handlers,
I/O buffers and classes for filesystem access with PhysicsFS support,
log writer, JSON file manipulation class, math functions and math classes.
ozDynamics library is a rigid body physics engine used in OpenZone engine.
ozEngine library contains basic engine building blocks for OpenZone game.
ozFactory library contains functions for building OpenZone assets.

%package -n liboz-devel
Summary:        Headers for liboz
Group:          Development/Libraries
License:        zlib
Requires:       liboz = %{version}

%description -n liboz-devel
This package contains header files for OpenZone ozCore, ozEngine and ozFactory
libraries.

%package data
Summary:        OpenZone game data
License:        Custom
BuildArch:      noarch
Requires:       %{name} = %{version}

%description data
Game data for OpenZone. Includes tutorial, test world and Cviček mission.

%prep
%setup -q -b 1

%build
mkdir -p build && cd build

cmake \
  -D CMAKE_INSTALL_PREFIX=/usr \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_CXX_FLAGS="-msse3 -mfpmath=sse" \
  -D CMAKE_CXX_FLAGS_RELEASE="-Ofast -flto" \
  -D CMAKE_AR="/usr/bin/gcc-ar" \
  -D CMAKE_RANLIB="/usr/bin/gcc-ranlib" \
  -D BUILD_SHARED_LIBS=1 \
  -D OZ_TOOLS=1 \
  ..

make %{?_smp_mflags}

%install
rm -rf "$RPM_BUILD_ROOT"

( cd build && cmake -D CMAKE_INSTALL_PREFIX="$RPM_BUILD_ROOT" -P cmake_install.cmake )

if [[ %{_libdir} != /usr/lib ]]; then
  sed -ri 's|libdir=.*|libdir=%{_libdir}|' "$RPM_BUILD_ROOT"/usr/lib/pkgconfig/*
  mv "$RPM_BUILD_ROOT"/usr/lib "$RPM_BUILD_ROOT"%{_libdir}
fi

install -dm755 "$RPM_BUILD_ROOT"%{_datadir}/openzone
install -m644 share/openzone/*.zip "$RPM_BUILD_ROOT"%{_datadir}/openzone

%files
%defattr(-, root, root, -)
%{_bindir}/*
%{_datadir}/applications
%{_datadir}/pixmaps
%doc AUTHORS COPYING README.md ChangeLog.md doc/*.html doc/licences

%files -n liboz
%defattr(-, root, root, -)
%{_libdir}/libozCore.so*
%{_libdir}/libozEngine.so*
%{_libdir}/libozFactory.so*
%doc src/ozCore/COPYING

%files -n liboz-devel
%defattr(-, root, root, -)
%{_libdir}/pkgconfig/ozCore.pc
%{_libdir}/pkgconfig/ozEngine.pc
%{_libdir}/pkgconfig/ozFactory.pc
%{_includedir}/ozCore
%{_includedir}/ozEngine
%{_includedir}/ozFactory
%doc src/ozCore/COPYING

%files data
%defattr(-, root, root, -)
%{_datadir}/openzone
