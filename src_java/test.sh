#!/usr/bin/bsh
JAVA_HOME=/QOpenSys/QIBM/ProdData/JavaVM/jdk80/64bit export JAVA_HOME
JCKHOME=/JCK/JCK-runtime-8b/
#java -Xdump:java:events=throw,filter=java/lang/UnsatisfiedLinkError -cp $JCKHOME/lib/jtjck.jar:$JCKHOME/lib/javatest.jar:$JCKHOME/classes javasoft.sqe.tests.api.java.nio.charset.Charset.UTF16Tests
java -cp $JCKHOME/lib/jtjck.jar:$JCKHOME/lib/javatest.jar:$JCKHOME/classes javasoft.sqe.tests.api.java.nio.charset.Charset.UTF16Tests

