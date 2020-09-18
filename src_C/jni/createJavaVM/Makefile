# This definition makes JAVA_HOME refer to the JDK specified in your PATH.
# You can change this to refer to a specific directory
#JAVA_HOME=/QOpenSys/QIBM/ProdData/JavaVM/jdk${JDK}/64bit
#JAVA_HOME=/qopensys/home/zhanggan/build_12_04/openj9-openjdk-jdk11/build/aix-ppc64-normal-server-release/jdk
JAVA_HOME=/QOpenSys/home/zhanggan/java${JDK}s/64bit
all:invoke Prog.class 
	QIBM_JVM_ILE_HOME=QJVM${JDK}64 LIBPATH=${JAVA_HOME}/lib:${JAVA_HOME}/lib/compressedrefs:${JAVA_HOME}/jre/lib/ppc64/:${JAVA_HOME}/jre/lib/ppc64/compressedrefs invoke Prog
#	LIBPATH=${JAVA_HOME}/lib:${JAVA_HOME}/lib/compressedrefs:/qopensys/home/zhanggan/build_12_04/openj9-openjdk-jdk11/openj9/runtime invoke Prog
#	LIBPATH=${JAVA_HOME}/lib:${JAVA_HOME}/lib/compressedrefs:${JAVA_HOME}/jre/lib/ppc64/:${JAVA_HOME}/jre/lib/ppc64/compressedrefs invokei -cp. QIBMHello
invoke:
	xlC -g -brwexec_must -blibsuff:so -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/aix -L$(JAVA_HOME)/lib/j9vm -L$(JAVA_HOME)/jre/lib/ppc64/j9vm  -ljvm -q64 -qarch=auto -o invoke invoke.cpp
#	xlc -g -brwexec_must -blibsuff:so -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/aix -L$(JAVA_HOME)/lib/j9vm -L$(JAVA_HOME)/jre/lib/ppc64/j9vm  -ljvm -q64 -qarch=auto -o invokei pase_invoke.c
Prog.class:
	$(JAVA_HOME)/bin/javac Prog.java
clean:
	rm -f *.class invoke *.o invokei  *.trc.* core
