#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

#include "jni.h"

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

JNIEXPORT jstring JNICALL Java_SignalHandlerExample_signalReg
  (JNIEnv *env, jobject jobj){
  	jstring answer;
  	gEnv = env;
	printf("zg.tid:%ld,SIGTERM register.1\n",GETTID);
	void(* pSysHandle)(int)=NULL;
	pSysHandle = signal(SIGTERM, sighandler);
	if( pSysHandle == SIG_DFL || pSysHandle == SIG_IGN || pSysHandle == SIG_ERR){
		printf("signal ERROR: pSysHandle=%p\n",pSysHandle,SIG_DFL);
		exit(-1);
	}
	printf("zg.tid:%ld,SIGTERM register.2\n",GETTID);
	for(int i =0;i<20;i++){
		sleep(1);
		printf("*\n");
	}
	char *msg = strdup("SIGTERM registered in JNI.");
	answer = (*env)->NewStringUTF(env, msg);
	free(msg);
	return answer;
  }
JNIEXPORT jstring JNICALL Java_SignalHandlerExample_getinfo(JNIEnv *env, jobject jobj) {
	jstring answer;
	unsigned long *pinNative = (unsigned long *)((char *) env + 1376);
	unsigned long *pprivateflags = (unsigned long *)((char *) env +220);
	
	printf("zg.in JNI[tid:%ld]: inNative=%lu,privateflags=%lu\n",GETTID,*pinNative,*pprivateflags);
	//*pinNative = 0;	
	//printf("in JNI[tid:%ld]: inNative=%lu,privateflags=%lu\n",GETTID,*pinNative,*pprivateflags);
	for(int i =0;i<20;i++){
		sleep(1);
		printf("+\n");
	}

	char *msg = strdup("hello world from jni!");
	answer = (*env)->NewStringUTF(env, msg);
	free(msg);
	return answer;
}
void sighandler(int s){
	printf("zg.Drop in sighandler.input=%d\n",s);
    unsigned long *pinNative = (unsigned long *)((char *) gEnv + 1376);
   	unsigned long *pprivateflags = (unsigned long *)((char *) gEnv +220);
	
   	printf("zg.in sighandler[tid:%ld]: inNative=%lu,privateflags=%lu\n",GETTID,*pinNative,*pprivateflags);
	*pinNative = 1L;
   	printf("zg.in sighandler[tid:%ld]: inNative=%lu,privateflags=%lu\n",GETTID,*pinNative,*pprivateflags);
	sleep(3);
	printf("zg.returned from sighandler after 3 seconds\n");
}
void sighandlermain(int s){
	printf("Drop in sighandler.input=%d\n",s);
	sleep(3);
	printf("returned from sighandler after 3 seconds\n");
}