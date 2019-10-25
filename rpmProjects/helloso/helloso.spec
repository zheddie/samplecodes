Name: helloso
Version: 2.0
Release: 0
License: LGPLv3
Summary: test or rpm build on ibmi.
Url: http://zg.com

Source0: helloso2-2.0.tar.Z

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
mkdir helloso2-2.0
cp %{SOURCE0} helloso2-2.0/
cd helloso2-2.0 && tar zxf ${SOURCE0} && cd ..


%build
cd helloso2-2.0
make PREFIX=%{buildroot} install

%files -n helloso2
#%defattr(-, qsys, *none)
${_libdir}/libhello.so.1*
${_libdir}/libworld.so.1*
${_bindir}/mytest

%files devel
#%defattr(-, qsys, *none)
${_incdir}/world.h
${_libdir}/libworld.so
${_libdir}/libhello.so

%changelog
%clean
