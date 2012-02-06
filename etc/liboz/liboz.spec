Name:           liboz
Version:        0.2.80
Release:        1
Summary:        OpenZone core library
URL:            https://github.com/ducakar/openzone/
License:        MIT
Group:          System Environment/Libraries
Packager:       Davorin Učakar <davorin.ucakar@gmail.com>
Source:         openzone-src-%{version}.tar.xz

%package devel
Summary:        Headers for OpenZone core library
Group:          Development/Libraries
Requires:       %{name} = %{version}

%description
Library provides facilities like container templates, array utilities,
string class, memory manager with memory leak tracing, crash handlers,
I/O buffers and classes for filesystem access with PhysicsFS support,
log writer, configuration file manipulation class, math functions and
linear algebra classes.
Library is primarily intended for use in OpenZone game engine.

%description devel
Library provides facilities like container templates, array utilities,
string class, memory manager with memory leak tracing, crash handlers,
I/O buffers and classes for filesystem access with PhysicsFS support,
log writer, configuration file manipulation class, math functions and
linear algebra classes.
Library is primarily intended for use in OpenZone game engine.

%prep
tar xf %{_sourcedir}/openzone-src-%{version}.tar.xz

%build
mkdir -p openzone-build && cd openzone-build

cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DOZ_INSTALL_LIBOZ=1 -DOZ_INSTALL_CLIENT=0 -DOZ_INSTALL_TOOLS=0 -DOZ_INSTALL_INFO=0 -DOZ_INSTALL_MENU=0 \
  ../openzone
make -j4

%install
rm -rf $RPM_BUILD_ROOT
cd openzone-build

make install DESTDIR=$RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_libdir}/liboz.so*
%doc %{_defaultdocdir}/%{name}-%{version}

%files devel
%defattr(-,root,root)
%{_includedir}/oz
%{_libdir}/pkgconfig
