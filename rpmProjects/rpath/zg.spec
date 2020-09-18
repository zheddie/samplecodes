Name: zgtest
Version: 1.0
Release: 0
License: LGPLv3
Summary: test or rpm build on ibmi.
Url: http://zg.com

Source0: test.c
Source1: hello.c
Source2: world.c
Source3: world.h
Source4: makefile


%description
RPM build demo.

%package -n zgtest1
Summary: zgtest library
Provides: %{name} = %{version}

%description -n zgtest1
zgtest libraries

%package devel
Summary: zgtest development files
Requires: %{name} = %{version}

%description devel
zgtest development files


%prep
mkdir zgtest-1.0
cp %{SOURCE0} zgtest-1.0/
cp %{SOURCE1} zgtest-1.0/
cp %{SOURCE2} zgtest-1.0/
cp %{SOURCE3} zgtest-1.0/
cp %{SOURCE4} zgtest-1.0/

%build
cd zgtest-1.0
make PREFIX=%{buildroot}/zgroot/ install

%files -n zgtest1
#%defattr(-, qsys, *none)
/zgroot/lib/libhello.so.1*
/zgroot/lib/libworld.so.1*
/zgroot/bin/mytest

%files devel
#%defattr(-, qsys, *none)
/zgroot/include/world.h
/zgroot/lib/libworld.so
/zgroot/lib/libhello.so

%changelog
%clean
