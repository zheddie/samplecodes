Name:	hello1	
Version:	1.0
Release:	0
Summary:	Hello of RPM.

License:	GPL
URL:		http://home.com
Source0:	hello1-1.0.tar.Z


%description
Hello of RPM.

%prep
%setup -q -n hello1-%{version}


#%build
#make PREFIX=%{buildroot}/zgroot %{?_smp_mflags}


%install
make PREFIX=%{buildroot}/zgroot install


%files
/zgroot/bin/hello


%changelog
%clean
