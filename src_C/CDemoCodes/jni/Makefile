# This definition makes JAVA_HOME refer to the JDK specified in your PATH.
# You can change this to refer to a specific directory
JAVA_HOME=/opt/ibm/java2-i386-50
#JAVA_HOME=/opt/IBMJava2-142
all:    
	invoke Prog.class
Proc:
	gcc -g -I$(JAVA_HOME)/include -c -o Proc.o Proc.c
	gcc -shared -o libPro.so Proc.o
invoke:
	g++ -g -I$(JAVA_HOME)/include -L$(JAVA_HOME)/jre/bin/classic -L$(JAVA_HOME)/jre/bin -ljvm -o invoke invoke.cpp
	
Prog.class:
	$(JAVA_HOME)/bin/javac Prog.java
clean:
	rm -f *.class invoke *.o
