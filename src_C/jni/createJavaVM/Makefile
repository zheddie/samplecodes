# This definition makes JAVA_HOME refer to the JDK specified in your PATH.
# You can change this to refer to a specific directory
JAVA_HOME=/opt/ibm/java2-i386-50
all:    
	invoke Prog.class
invoke:
	xlc -g -I$(JAVA_HOME)/include -L$(JAVA_HOME)/jre/bin/j9vm -L$(JAVA_HOME)/jre/bin -ljvm -q64 -qarch=auto -o invoke invoke.cpp
Prog.class:
	$(JAVA_HOME)/bin/javac Prog.java
clean:
	rm -f *.class invoke *.o
