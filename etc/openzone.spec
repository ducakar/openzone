Name:           openzone
Version:        0.3.0
Release:        1
Summary:        Simple cross-platform FPS/RTS game engine
URL:            https://github.com/ducakar/openzone/
License:        GPLv3+
Group:          Amusements/Games
Packager:       Davorin Učakar <davorin.ucakar@gmail.com>
Source:         openzone-src-%{version}.tar.xz
Source1:        ozbase.zip
Source2:        openzone.zip

%description
OpenZone is a relatively simple cross-platform game engine, suitable for FPS,
RTS and RPG genres. It is strictly divided into several layers. Back-end runs
a complete simulation of a world (physics, object handlers, controllers, AI)
and front-end that renders it and enables the player to manipulate with the
simulated world.
This package only includes engine and essential data needed for engine to run.
Game data and missions packages must be installed separately.

%package tools
Summary:        Tools for building game data for OpenZone engine
Group:          Development/Tools

%description tools
ozBase tool for building game data packages for OpenZone engine and some other
miscellaneous tools.

%package data
Summary:        OpenZone game data
License:        Custom
Group:          Amusements/Games
Requires:       %{name} = %{version}

%description data
Game data for OpenZone. Includes tutorials, testing world and cviček mission.

%prep
tar xf %{_sourcedir}/openzone-src-%{version}.tar.xz

%build
mkdir -p openzone-build && cd openzone-build

cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DOZINST_LIBOZ=0 -DOZINST_CLIENT=1 -DOZINST_TOOLS=1 -DOZINST_INFO=1 ../openzone
make -j4

%install
rm -rf $RPM_BUILD_ROOT
cd openzone-build

make install DESTDIR=$RPM_BUILD_ROOT
mv $RPM_BUILD_ROOT/usr/share/doc/openzone $RPM_BUILD_ROOT/%{_defaultdocdir}/%{name}-%{version}
install -Dm644 %{_sourcedir}/ozbase.zip $RPM_BUILD_ROOT/%{_datadir}/openzone/ozbase.zip
install -Dm644 %{_sourcedir}/openzone.zip $RPM_BUILD_ROOT/%{_datadir}/openzone/openzone.zip

%files
%defattr(-,root,root)
%{_bindir}/openzone
%{_datadir}/share/applications/*
%{_datadir}/share/icons/*
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
