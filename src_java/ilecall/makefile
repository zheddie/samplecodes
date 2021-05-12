PGMNAME=callILENative
ILEMOD=ZILEM
ILESRV=ZILENAT
LIBNAME=GAVIN
PWD=$(shell pwd)
#JDKHOME=../openj9-openjdk-jdk11/build/linux-x86_64-normal-server-slowdebug/jdk
OMODE?=32
JDKHOME=/QOpenSys/QIBM/ProdData/JavaVM/jdk80/${OMODE}bit/
XLC_CMD=/QOpenSys/xlcpp/opt/IBM/xlC/13.1.3/bin/xlC
javac:
	${JDKHOME}/bin/javac  -h . ${PGMNAME}.java
compile:
	${XLC_CMD} -q${OMODE} -o ${PGMNAME}.o -fPIC -I${JDKHOME}/include -I${JDKHOME}/include/linux -I${JDKHOME}/../vm/oti -I${JDKHOME}/../vm/include -I${JDKHOME}/../vm/omr/include_core -DAIXPPC -c ${PGMNAME}.c
link:
	${XLC_CMD}  -q${OMODE} -qmkshrobj -o lib${PGMNAME}.so -lpthread ${PGMNAME}.o
ilecompile:
	system -e "CRTCMOD MODULE(${LIBNAME}/${ILEMOD}) SRCSTMF('${PWD}/${PGMNAME}.c')"
	system -e "CRTSRVPGM2 SRVPGM(${LIBNAME}/${ILESRV}) MODULE(GAVIN/${ILEMOD}) EXPORT(*ALL) STGMDL(*SNGLVL)"
ilerun:
	${JDKHOME}/bin/java -Djava.library.path=/QSYS.LIB/${LIBNAME}.LIB/ ${PGMNAME}
run:
	#${JDKHOME}/bin/java -Djava.library.path=/QOpenSys/home/zhanggan/worktemp/jnipase ${PGMNAME}
	${JDKHOME}/bin/java -Djava.library.path=. ${PGMNAME}
tracerun:
	#${JDKHOME}/bin/java -Xtrace:print={omrport.475-480,j9vm.536,j9vm.231,j9vm.127,j9vm.229,j9vm.230} -Djava.library.path=/QOpenSys/home/zhanggan/worktemp/jnipase ${PGMNAME}
	${JDKHOME}/bin/java -Xtrace:print={omrport.475-480,j9vm.536,j9vm.231,j9vm.127,j9vm.229,j9vm.230} -Djava.library.path=. ${PGMNAME}

debugrun:
	ZDEBUG=1 ZODEBUG=VMAccess.cpp ${JDKHOME}/bin/java -Djava.library.path=. ${PGMNAME}
main: compile
	${XLC_CMD} -o ${PGMNAME} ${PGMNAME}.o -lpthreads
	
	
all: javac compile link
clean:
	rm ${PGMNAME}*.class ${PGMNAME}.o lib${PGMNAME}.so ${PGMNAME}.h 
