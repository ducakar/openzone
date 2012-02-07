Name:           openzone
Version:        0.2.80
Release:        1
Summary:        Simple cross-platform FPS/RTS game engine
URL:            https://github.com/ducakar/openzone/
License:        GPLv3+
Group:          Amusements/Games
Packager:       Davorin Učakar <davorin.ucakar@gmail.com>
Source:         openzone-src-%{version}.tar.xz
Source1:        openzone-data-ozbase-%{version}.tar.xz
Source2:        openzone-data-openzone-%{version}.tar.xz

%package tools
Summary:        Tools for building game data for OpenZone engine
Group:          Development/Tools

%package data
Summary:        OpenZone game data
License:        Custom
Group:          Amusements/Games
Requires:       %{name} = %{version}

%description
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

%prep
tar xf %{_sourcedir}/openzone-src-%{version}.tar.xz
tar xf %{_sourcedir}/openzone-data-ozbase-%{version}.tar.xz
tar xf %{_sourcedir}/openzone-data-openzone-%{version}.tar.xz

%build
mkdir -p openzone-build && cd openzone-build

cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DOZ_INSTALL_LIBOZ=0 -DOZ_INSTALL_CLIENT=1 -DOZ_INSTALL_TOOLS=1 -DOZ_INSTALL_INFO=1 -DOZ_INSTALL_MENU=1 \
  ../openzone
make -j4

%install
rm -rf $RPM_BUILD_ROOT
cd openzone-build

make install DESTDIR=$RPM_BUILD_ROOT

install -Dm644 ../ozbase.zip $RPM_BUILD_ROOT/%{_datadir}/openzone/ozbase.zip
install -Dm644 ../openzone.zip $RPM_BUILD_ROOT/%{_datadir}/openzone/openzone.zip

%files
%defattr(-,root,root)
%{_bindir}/openzone
# %{_datadir}/share/applications
# %{_datadir}/share/pixmaps
%dir %{_datadir}/openzone
%{_datadir}/openzone/ozbase.zip
%doc %{_defaultdocdir}/%{name}-%{version}

%files tools
%defattr(-,root,root)
%{_bindir}/ozBuild
%{_bindir}/ozFormat

%files data
%defattr(-,root,root)
%dir %{_datadir}/openzone
%{_datadir}/openzone/openzone.zip
