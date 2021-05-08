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
%setup -q -n helloso2-%{version}

%build
make PREFIX=/usr VERSION=2 all
make PREFIX=%{buildroot}/usr/ VERSION=2 install

%files -n helloso2
#%defattr(-, qsys, *none)
%{_libdir}/libhello.so.2
%{_libdir}/libworld.so.2
%{_bindir}/mytest

%files devel
#%defattr(-, qsys, *none)
%{_includedir}/world.h
%{_libdir}/libhello.so
%{_libdir}/libworld.so

%changelog
%clean
