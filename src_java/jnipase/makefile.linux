PGMNAME=PaseExample1
JDKHOME=../openj9-openjdk-jdk11/build/linux-x86_64-normal-server-slowdebug
javac:
	javac  -h . ${PGMNAME}.java
compile:
	gcc -o ${PGMNAME}.o -fPIC -I${JDKHOME}/jdk/include/ -I${JDKHOME}/jdk/include/linux -I${JDKHOME}/vm/oti -I${JDKHOME}/vm/include -I${JDKHOME}/vm/omr/include_core -c ${PGMNAME}.c
link:
	gcc -shared -o lib${PGMNAME}.so ${PGMNAME}.o
run:
	${JDKHOME}/jdk/bin/java -Djava.library.path=/root/openj9root/jnipase/ ${PGMNAME}
debugrun:
	ZDEBUG=1 ZODEBUG=VMAccess.cpp ${JDKHOME}/jdk/bin/java -Djava.library.path=/root/openj9root/jnipase/ ${PGMNAME}
	
all: javac compile link
clean:
	rm ${PGMNAME}*.class ${PGMNAME}.o lib${PGMNAME}.so ${PGMNAME}.h 
