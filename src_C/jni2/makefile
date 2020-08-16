JAVAHOME=/root/devroot/ibm-java-i386-80
PGMNAME=JavaNativeHelloWorld
javac:
	javac -h . ${PGMNAME}.java
compile:
	gcc -m32 -o libhelloworld.so -I${JAVAHOME}/include -I${JAVAHOME}/include/linux -shared ${PGMNAME}.cpp 
run:
	java -Djava.library.path=. ${PGMNAME}
main:
	gcc -m32 -o main -I${JAVAHOME}/include -I${JAVAHOME}/include/linux ${PGMNAME}.cpp 
all: javac compile
clean:
	rm ${PGMNAME}*.class ${PGMNAME}.o libhelloworld.so ${PGMNAME}.h
