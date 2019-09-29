#!/bin/bash
set -x
CURRENTDIR=`pwd`
HELLONAME=hello1-1.0
if [ "$1" == "hello" ]; then
	if [ "$2" == "rpmbuild" ]; then
		if [ ! -f ${HELLONAME}.tar.Z ]; then
			tar czf ${HELLONAME}.tar.Z ${HELLONAME}
		fi
		#tar czf ${HELLONAME}.tar.Z --exclude="*.spec" ${HELLONAME}
		cp ${HELLONAME}.tar.Z ~/rpmbuild/SOURCES/
		rpmbuild -ba ${HELLONAME}/hello.spec --buildroot ~/rpmbuild/BUILDROOT -vv
	fi
	
fi
