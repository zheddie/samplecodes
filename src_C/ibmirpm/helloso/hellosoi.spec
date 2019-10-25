%define insdest /QOpenSys/pkgs

Name: helloso
Version: 2.0
Release: 0
License: LGPLv3
Summary: test or rpm build on ibmi.
Url: http://zg.com

Source0: helloso2-2.0.tar.Z


#BuildRequires: ibm-build-tools
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

%define soversion 2
%build
make -f makefilei PREFIX=%{insdest} VERSION=%{soversion} all

%install
make -f makefilei PREFIX=%{buildroot}%{insdest} VERSION=%{soversion} install

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
