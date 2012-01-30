Name:           liboz
Version:        0.3.0
Release:        1
Summary:        OpenZone core library
URL:            https://github.com/ducakar/openzone/
License:        MIT
Group:          System Environment/Libraries
Packager:       Davorin Učakar <davorin.ucakar@gmail.com>

%description
Library provides facilities like container templates, array utilities,
string class, memory manager with memory leak tracing, crash handlers,
I/O buffers and classes for filesystem access with PhysicsFS support,
log writer, configuration file manipulation class, math functions and
linear algebra classes.
Library is primarily intended for use in OpenZone game engine.

%package devel
Summary:        OpenZone core library development files
Group:          Development/Libraries
Requires:       %{name} = %{version}

%description devel
Library provides facilities like container templates, array utilities,
string class, memory manager with memory leak tracing, crash handlers,
I/O buffers and classes for filesystem access with PhysicsFS support,
log writer, configuration file manipulation class, math functions and
linear algebra classes.
Library is primarily intended for use in OpenZone game engine.

%prep
if [ ! -d openzone ]; then
  git clone /home/davorin/Projects/openzone
  ( cd openzone && git checkout devel )
fi

( cd openzone && git pull )

%build
mkdir -p openzone-build && cd openzone-build

cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DOZINST_LIBOZ=1 -DOZINST_CLIENT=0 -DOZINST_TOOLS=0 -DOZINST_INFO=0 ../openzone
make -j4

%install
rm -rf $RPM_BUILD_ROOT
cd openzone-build

make install DESTDIR=$RPM_BUILD_ROOT

# Some distros use /usr/lib64 instead of /usr/lib.
if [ %{_libdor} != /usr/lib ]; then
  mv $RPM_BUILD_ROOT/usr/lib $RPM_BUILD_ROOT/%{_libdir}
fi
mv $RPM_BUILD_ROOT/usr/share/doc/liboz $RPM_BUILD_ROOT/%{_defaultdocdir}/%{name}-%{version}

libdir=%{_libdir}
includedir=%{_includedir}
sed "s|libdir=.*$|libdir=$libdir|" -i $RPM_BUILD_ROOT/%{_libdir}/pkgconfig/liboz.pc
sed "s|includedir=.*$|includedir=$includedir|" -i $RPM_BUILD_ROOT/%{_libdir}/pkgconfig/liboz.pc

%files
%defattr(-,root,root)
%{_libdir}/liboz.so*
%doc %{_defaultdocdir}/%{name}-%{version}

%files devel
%defattr(-,root,root)
%{_includedir}/oz
%{_libdir}/pkgconfig
