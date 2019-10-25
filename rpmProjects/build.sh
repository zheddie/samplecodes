#!/bin/bash
set -x
CURRENTDIR=`pwd`
HELLONAME=hello1-1.0
HELLOSONAME=helloso2-2.0
if [ "$1" == "hello" ]; then
	cd hello
        if [ "$2" == "unzipsrc" ]; then
                tar zxf ${HELLONAME}.tar.Z
        fi
        if [ "$2" == "zipsrc" ]; then
                tar czf ${HELLONAME}.tar.Z ${HELLONAME}
        fi
	if [ "$2" == "rpmbuildi" ]; then
		rm -rf ~/build.hello
		mkdir ~/build.hello
		PYTHONUNBUFFERED=true ./build.py --root ~/build.hello --rpm-cache-dir ~/rpm-cache -v --spec hello/hello.spec
	fi
	if [ "$2" == "rpmbuilds" ]; then
		PYTHONUNBUFFERED=true ./build.py --skipchroot --root ~/build.hello --rpm-cache-dir ~/rpm-cache -v --spec hello/hello.spec
	fi

	if [ "$2" == "rpmbuildl" ]; then
		if [ ! -f ${HELLONAME}.tar.Z ]; then
			tar czf ${HELLONAME}.tar.Z ${HELLONAME}
		fi
		#tar czf ${HELLONAME}.tar.Z --exclude="*.spec" ${HELLONAME}
		cp ${HELLONAME}.tar.Z ~/rpmbuild/SOURCES/
		rpmbuild -ba ${HELLONAME}/hello.spec --buildroot ~/rpmbuild/BUILDROOT -vv
	fi
	
fi
if [ "$1" == "helloso" ]; then
	cd helloso
	if [ "$2" == "zipsrc" ]; then
		if [ -f ${HELLOSONAME}.tar.Z ]; then
			rm -rf ${HELLOSONAME}.tar.Z
		fi
		tar czf ${HELLOSONAME}.tar.Z ${HELLOSONAME}
	fi
	if [ "$2" == "rpmbuildi" ]; then
		rm -rf ~/build.helloso
		mkdir ~/build.helloso
		PYTHONUNBUFFERED=true ./build.py --root ~/build.helloso --rpm-cache-dir ~/rpm-cache -v --spec helloso/helloso.spec
	fi
	if [ "$2" == "rpmbuilds" ]; then
		PYTHONUNBUFFERED=true ./build.py --skipchroot --root ~/build.helloso --rpm-cache-dir ~/rpm-cache -v --spec helloso/helloso.spec
	fi
	if [ "$2" == "rpmbuild" ]; then
		if [ -f ${HELLOSONAME}.tar.Z ]; then
			rm -rf ${HELLOSONAME}.tar.Z
		fi
		tar czf ${HELLOSONAME}.tar.Z ${HELLOSONAME}
		cp ${HELLOSONAME}.tar.Z ~/rpmbuild/SOURCES/
		rpmbuild -ba helloso.spec --buildroot ~/rpmbuild/BUILDROOT -vv
	fi
	
fi
