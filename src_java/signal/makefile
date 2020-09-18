PGMNAME=SignalHandlerExample
#JDKHOME=../openj9-openjdk-jdk11/build/linux-x86_64-normal-server-slowdebug/jdk
OMODE?=32
JDKHOME=/QOpenSys/QIBM/ProdData/JavaVM/jdk80/${OMODE}bit/
javac:
	${JDKHOME}/bin/javac  -h . ${PGMNAME}.java
compile:
	xlC -q${OMODE} -o ${PGMNAME}.o -fPIC -I${JDKHOME}/include -I${JDKHOME}/include/linux -I${JDKHOME}/../vm/oti -I${JDKHOME}/../vm/include -I${JDKHOME}/../vm/omr/include_core -DAIXPPC -c ${PGMNAME}.c
link:
	xlC  -q${OMODE} -qmkshrobj -o lib${PGMNAME}.so -lpthread ${PGMNAME}.o
run:
	${JDKHOME}/bin/java -Djava.library.path=${PWD} ${PGMNAME}
tracerun:
	${JDKHOME}/bin/java -Xtrace:print={omrport.475-480,j9vm.536,j9vm.231,j9vm.127,j9vm.229,j9vm.230} -Djava.library.path=/QOpenSys/home/zhanggan/worktemp/jnipase ${PGMNAME}

debugrun:
	ZDEBUG=1 ZODEBUG=VMAccess.cpp ${JDKHOME}/bin/java -Djava.library.path=. ${PGMNAME}
main: compile
	xlC -o ${PGMNAME} ${PGMNAME}.o -lpthreads
	
	
all: javac compile link
clean:
	rm ${PGMNAME}*.class ${PGMNAME}.o lib${PGMNAME}.so ${PGMNAME}.h 
