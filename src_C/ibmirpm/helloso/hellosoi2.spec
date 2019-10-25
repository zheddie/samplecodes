%define insdest /QOpenSys/pkgs
%define soversion 2

Name: helloso
Version: 2.0
Release: 0
License: LGPLv3
Summary: test or rpm build on ibmi.
Url: http://zg.com

Source0: helloso2-2.0.tar.Z


#BuildRequires: ibm-build-tools
BuildRequires: cmake
BuildRequires: pase-build-tools

%description
RPM build demo.

%package -n helloso2
Summary:  helloso library
Provides: %{name} = %{version}

%description -n helloso2
helloso2 libraries

%package devel
Summary: helloso2 development files
Requires: %{name} = %{version}

%description devel
helloso2 development files

%prep
%setup -q -n helloso2-%{version}


%build
mkdir build && cd build
cmake -DCMAKE_SHARED_LINKER_FLAGS="-Wl,-blibpath:/QOpenSys/pkgs/lib:/QOpenSys/usr/lib" -DCMAKE_EXE_LINKER_FLAGS="-Wl,-blibpath:/QOpenSys/pkgs/lib:/QOpenSys/usr/lib" -DCMAKE_INSTALL_PREFIX=%{buildroot}/QOpenSys/pkgs/ ..
make world
#make install
#cd %{buildroot}/QOpenSys/pkgs/lib/
mv libworld.so libworld.so.2
svr4-shrlib libworld.so.2 libworld.so.2 ppc64
ln -s libworld.so.2 libworld.so
make hello
mv libhello.so libhello.so.2
svr4-shrlib libhello.so.2 libhello.so.2 ppc64
ln -s libhello.so.2  libhello.so
make mytest
cp libhello.so.2 %{buildroot}/QOpenSys/pkgs/lib/
cp libworld.so.2 %{buildroot}/QOpenSys/pkgs/lib/
cp mytest %{buildroot}/QOpenSys/pkgs/bin/
cp ../world.h %{buildroot}/QOpenSys/pkgs/include/
cp -d libhello.so %{buildroot}/QOpenSys/pkgs/lib/
cp -d libworld.so %{buildroot}/QOpenSys/pkgs/lib/


%files -n helloso2
%defattr(-, qsys, *none)
%{_libdir}/libhello.so.*
%{_libdir}/libworld.so.*
%{_bindir}/mytest

%files devel
#%defattr(-, qsys, *none)
%{_includedir}/world.h
%{_libdir}/libhello.so
%{_libdir}/libworld.so

%changelog
%clean
