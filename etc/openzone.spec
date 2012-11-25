Name:           openzone
Version:        0.3.81
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
BuildRequires:  pulseaudio-libs-devel
BuildRequires:  physfs-devel
BuildRequires:  lua-devel
BuildRequires:  SDL
BuildRequires:  SDL_ttf
BuildRequires:  mesa-libGL-devel
BuildRequires:  openal-soft-devel
BuildRequires:  libvorbis-devel
BuildRequires:  freeimage-devel

%description
OpenZone is a relatively simple cross-platform game engine, suitable for FPS,
RTS and RPG genres. It is strictly divided into several layers. Back-end runs
a complete simulation of a world (physics, object handlers, controllers, AI)
and front-end that renders it and enables the player to manipulate with the
simulated world.

%package -n liboz
Summary:        OpenZone Core and Dynamics libraries
Group:          System Environment/Libraries
License:        zlib

%description -n liboz
OpenZone Core Library provides facilities like container templates, array
utilities, string class, memory manager with memory leak tracing, crash
handlers, I/O buffers and classes for filesystem access with PhysicsFS support,
log writer, JSON file manipulation class, math functions and linear algebra
classes.
OpenZone Dynamics Library is a simple rigid body physics engine used in
OpenZone game.

%package -n liboz-devel
Summary:        Headers for OpenZone Core and Dynamics libraries
Group:          Development/Libraries
License:        zlib
Requires:       liboz = %{version}

%description -n liboz-devel
This package contains header files OpenZone Core and Dynamics libraries.

%package data
Summary:        OpenZone game data
License:        Custom
BuildArch:      noarch
Requires:       %{name} = %{version}

%description data
Game data for OpenZone. Includes tutorial, test world and cviček mission.

%prep
%setup -q -b 1

%build
cd build

cmake \
  -D CMAKE_INSTALL_PREFIX=/usr \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_CXX_FLAGS="-msse3 -mfpmath=sse" \
  -D CMAKE_CXX_FLAGS_RELEASE="-Ofast -flto" \
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
%defattr(-, root, root, -)
%{_bindir}/*
%{_datadir}/applications
%{_datadir}/pixmaps
%doc AUTHORS COPYING README.md ChangeLog.md doc/*.html doc/licences

%files -n liboz
%defattr(-, root, root, -)
%{_libdir}/libozCore.so*
%{_libdir}/libozDynamics.so*
%doc src/ozCore/COPYING

%files -n liboz-devel
%defattr(-, root, root, -)
%{_libdir}/pkgconfig/ozCore.pc
%{_libdir}/pkgconfig/ozDynamics.pc
%{_includedir}/ozCore
%{_includedir}/ozDynamics
%doc src/ozCore/COPYING

%files data
%defattr(-, root, root, -)
%{_datadir}/openzone
