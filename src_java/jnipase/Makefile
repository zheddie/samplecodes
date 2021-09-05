PGMNAME=PaseExample1
JDKHOME=/QOpenSys/QIBM/ProdData/JavaVM/jdk80/32bit
JAVA_HOME=/QOpenSys/QIBM/ProdData/JavaVM/jdk80/32bit
javac:
	${JDKHOME}/bin/javac  -h . ${PGMNAME}.java
compile:
	/qopensys/pkgs/bin/gcc -maix32 -o ${PGMNAME}.o -fPIC -I${JDKHOME}/include/ -c ${PGMNAME}.c
link:
	/qopensys/pkgs/bin/gcc -maix32 -shared -o lib${PGMNAME}.so ${PGMNAME}.o
xlc:
	xlc -q32 -o ${PGMNAME}.o -fPIC -I${JDKHOME}/include/ -c ${PGMNAME}.c
	xlc -q32 -qmkshrobj -o lib${PGMNAME}.so ${PGMNAME}.o
run:
	${JDKHOME}/bin/java -Djava.library.path=. ${PGMNAME}
debugrun:
	ZDEBUG=1 ZODEBUG=VMAccess.cpp ${JDKHOME}/bin/java -Djava.library.path=. ${PGMNAME}
	
all: javac compile link
clean:
	rm ${PGMNAME}*.class ${PGMNAME}.o lib${PGMNAME}.so ${PGMNAME}.h 
