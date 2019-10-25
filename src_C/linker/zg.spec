Name: zgtest
Version: 1.0
Release: 0
License: LGPLv3
Summary: test or rpm build on ibmi.
Url: http://zg.com

Source0: main.c
Source1: hello.h
Source2: hello.c
Source3: makefile


%description
RPM build demo.

%package -n zgtest1
Summary: libhello library
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
cp %{SOURCE0} zgtest-1.0
cp %{SOURCE1} zgtest-1.0
cp %{SOURCE2} zgtest-1.0
cp %{SOURCE3} zgtest-1.0


%build
cd zgtest-1.0
make all
cp 
cd ../../


%files -n libzmq4
%defattr(-, qsys, *none)

%{_libdir}/libzmq.so.4*
%{_libdir}/libzmq.a
%{_libdir}/pkgconfig/libzmq.pc
%{_bindir}/local_lat
%{_bindir}/remote_lat
%{_bindir}/local_thr
%{_bindir}/remote_thr
%{_bindir}/inproc_lat
%{_bindir}/inproc_thr
%{_datadir}/zmq/AUTHORS.txt
%{_datadir}/zmq/COPYING.txt
%{_datadir}/zmq/COPYING.LESSER.txt
%{_datadir}/zmq/NEWS.txt

%files devel
%defattr(-, qsys, *none)
%{_libdir}/libzmq.so
%{_includedir}/zmq.h
%{_includedir}/zmq_utils.h
%{_datadir}/cmake/ZeroMQ/ZeroMQTargets.cmake
%{_datadir}/cmake/ZeroMQ/ZeroMQTargets-release.cmake
%{_datadir}/cmake/ZeroMQ/ZeroMQConfig.cmake
%{_datadir}/cmake/ZeroMQ/ZeroMQConfigVersion.cmake


%changelog
%clean
