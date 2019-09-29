/*
* supertest: put com/ibm/iTestJava/envvar/JPAP.c  
* supertest: rcmd crtcmod module($library/$module) srcstmf('$file')
* supertest: rcmd crtpgm pgm($library/$pgm) module($library/$module) actgrp(*caller)
* supertest: rcmd savobj obj($pgm) lib($library) dev(*SAVF) objtype(*PGM) savf($library/SAVF) clear(*ALL)  
*/ 

/* File createjvm.C created by Jim Kryka,050142 at 13:33:10 on Fri Aug 13 2004. */

/*

CRTCPPMOD MODULE(KRYKA/CREATEJVM) SRCFILE(KRYKA/QCSRC) TERASPACE(*YES)

 CRTCPPMOD MODULE(KRYKA/CREATEJVM) SRCFILE(KRYKA/QCSRC) DBGVIEW(*SOURCE) TERASPACE(*YES) 

CRTPGM PGM(KRYKA/CREATEJVM)

 STRDBG PGM(KRYKA/CREATEJVM)
 enddbg


addenvvar  JAVA_HOME '/QOpenSys/QIBM/ProdData/JavaVM/jdk50/32bit'

call kryka/createjvm 




*/



#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "jni.h"
#include <qp2user.h>
#include <qp2shell2.h>



int main(int argc, char* argv[]) {
    int result;
    int rt;
    JavaVM* vm;
    JNIEnv*   env;
    jclass theClazz;
    jthrowable jt;

    JavaVMOption options[1];
    JavaVMInitArgs jvmArgs;

    printf("In main.\n");

    /* set LIBPATH env var for J9 */
    putenv(
"LIBPATH=/QOpenSys/QIBM/ProdData/JavaVM/jdk50/32bit/jre/bin:/QOpenSys/QIBM/ProdData/JavaVM/jdk50/32bit/jre/bin/j9vm"
	   );

    /* create 32 bit pase */
    /* this will create pase for the process and return,  keeping pase active */
    QP2SHELL2("/usr/lib/start32");

    /*  to create 64 bit pase, do this 
    QP2SHELL2("/usr/lib/start64");
    */

    printf("Pase has been started\n");

    /* the JavaVM and JNIEnv pointers are returned to these fields so
       we initialize them to null to ensure they are not garbage */
    vm = NULL;
    env = NULL;

    /* init the Jvm args to zero */
    memset(&jvmArgs, 0, sizeof(jvmArgs));
    /*  Set the version field of the initialization arguments. */
    jvmArgs.version = 0x00010002;

#pragma convert(819)
   /* we pass in the max heap size of 200M as a JVM option */
   options[0].optionString = "-Xmx200m";
#pragma convert(0)

    jvmArgs.options = options;
    jvmArgs.nOptions = 1;
    jvmArgs.ignoreUnrecognized = JNI_TRUE;

    /* create the javavm */
    result = JNI_CreateJavaVM(&vm, &env, &jvmArgs);

    printf("  vm = %p\n",vm);
    printf("  jnienv = %p\n",env);
    printf("  result = %i\n",result);

    /* do a findclass to make sure the JVM is working */
    theClazz = 0;
    jt = 0;
#pragma convert(819)
    theClazz = env->FindClass("java/lang/String");
    jt = env->ExceptionOccurred();
#pragma convert(0)
    printf("  clazz = %i\n",theClazz);
    printf("  jt = %i\n",jt);

    /* destroy  the JVM */
    rt = 0;
    rt = vm->DestroyJavaVM();
    printf("  rt from destroy = %i\n",rt);


    return 0;
}
