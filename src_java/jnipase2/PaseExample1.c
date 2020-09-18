/*
 *

 * This native method implements the getStringNative method of class
 * PaseExample1. It uses the JNI function CallObjectMethod to call 
 * back to the getStringCallback method of class PaseExample1.
 *
 * Compile this code in AIX or PASE for i to create module 'libPaseExample1.so'.
 *
 */

#include "PaseExample1.h"
//#include "j9nonbuilder.h"
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#ifdef LINUX
#include <sys/syscall.h>
#define GETTID syscall(SYS_gettid)
#endif
#ifdef AIXPPC
#include <pthread.h>
#define GETTID pthread_self()
#endif

static JNIEnv * gEnv = NULL;
void sighandler(int);
void sighandlermain(int);
int main(){
	signal(SIGTERM,sighandlermain);
	for(int i =0;i<15;i++){
	sleep(1);
	printf(".\n");
	}
	return(0);

}

/*
 * Class:     PaseExample1
 * Method:    getStringNative
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_PaseExample1_getStringNative(JNIEnv* env, jobject obj) {
   gEnv=env;
   char* methodName = "getStringCallback";
   char* methodSig = "()Ljava/lang/String;";
   printf("SIGTERM register.1\n");
   signal(SIGTERM, sighandler);
   printf("SIGTERM register.2\n");
//unsigned long *pinNative = (unsigned long *)((char *) env + 2360);
//unsigned long *pprivateflags = (unsigned long *)((char *) env +424);
   unsigned long *pinNative = (unsigned long *)((char *) env + 1376);
   unsigned long *pprivateflags = (unsigned long *)((char *) env +220);
   printf("in JNI[tid:%ld]: inNative=%lu,privateflags=%lu\n",GETTID,*pinNative,*pprivateflags);
   printf("\n");
/*
	J9VMThread *currentThread = (J9VMThread *)env;
	printf("in JNI:currentThread=%p,>inNative=%lu\n",currentThread,currentThread->inNative);
	J9JavaVM *vm = currentThread->javaVM;
	J9InternalVMFunctions *vmFuncs = vm->internalVMFunctions;
	vmFuncs->internalEnterVMFromJNI(currentThread);
	vmFuncs->setNativeOutOfMemoryError(currentThread, moduleName, messageNumber);
	vmFuncs->internalExitVMToJNI(currentThread);
 */
   for(int i =0;i<20;i++){
	sleep(1);
	printf("+\n");
	}
   jclass clazz = (*env)->GetObjectClass(env, obj); 
   printf("in JNI: inNative=%lu\n",*pinNative);
   jmethodID methodID = (*env)->GetMethodID(env, clazz, methodName, methodSig);
   printf("in JNI: inNative=%lu\n",*pinNative);
   jstring rt = (*env)->CallObjectMethod(env, obj, methodID);
   printf("in JNI: inNative=%lu\n",*pinNative);
   return (rt);
}
void sighandlermain(int s){
	printf("Drop in sighandler.input=%d\n",s);
	sleep(3);
	printf("returned from sighandler after 3 seconds\n");
}
void sighandler(int s){
	printf("Drop in sighandler.input=%d\n",s);
        unsigned long *pinNative = (unsigned long *)((char *) gEnv + 2360);
   	unsigned long *pprivateflags = (unsigned long *)((char *) gEnv +424);
	
   	printf("in sighandler[tid:%ld]: inNative=%lu,privateflags=%lu\n",GETTID,*pinNative,*pprivateflags);
//	*pinNative = 1L;
 //  	printf("in sighandler[tid:%ld]: inNative=%lu,privateflags=%lu\n",GETTID,*pinNative,*pprivateflags);
	sleep(3);
	printf("returned from sighandler after 3 seconds\n");
}
