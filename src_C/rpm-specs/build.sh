#!/bin/bash
set -x
CURRENTDIR=`pwd`
HELLONAME=hello1-1.0
HELLOSONAME=helloso2-2.0

if [ "$1" == "lapack" ]; then
	if [ "$2" == "rpmbuildi" ]; then
		PYTHONUNBUFFERED=true ./build.py --root ~/build.lapack --rpm-cache-dir ~/rpm-cache -v --spec lapack/lapack.spec
	fi
fi
if [ "$1" == "helloso" ]; then
	if [ "$2" == "zipsrc" ]; then
		cd helloso
		if [ -f ${HELLOSONAME}.tar.Z ]; then
			rm -rf ${HELLOSONAME}.tar.Z
		fi
		tar czf ${HELLOSONAME}.tar.Z ${HELLOSONAME}
	fi
	if [ "$2" == "rpmbuildi" ]; then
		rm -rf ~/build.helloso
		mkdir ~/build.helloso
		PYTHONUNBUFFERED=true ./build.py --root ~/build.helloso --rpm-cache-dir ~/rpm-cache -v --spec helloso/hellosoi.spec
	fi
	if [ "$2" == "rpmbuilds" ]; then
		PYTHONUNBUFFERED=true ./build.py --skipchroot --root ~/build.helloso --rpm-cache-dir ~/rpm-cache -v --spec helloso/hellosoi.spec
	fi
	if [ "$2" == "rpmbuildl" ]; then
		cp helloso/${HELLOSONAME}.tar.Z ~/rpmbuild/SOURCES/
		rpmbuild -ba helloso/helloso.spec --buildroot ~/rpmbuild/BUILDROOT -vv
	fi
	if [ "$2" == "rpmbuildl2" ]; then
		cp helloso/${HELLOSONAME}.tar.Z ~/rpmbuild/SOURCES/
		rpmbuild -ba helloso/helloso2.spec --buildroot ~/rpmbuild/BUILDROOT -vv
	fi	
fi
if [ "$1" == "hello" ]; then
        if [ "$2" == "zipsrc" ]; then
		cd hello
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
		cp hello/${HELLONAME}.tar.Z ~/rpmbuild/SOURCES/
		rpmbuild -ba hello/hello.spec --buildroot ~/rpmbuild/BUILDROOT -vv
	fi
	
fi
