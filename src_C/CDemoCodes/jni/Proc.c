#include <jni.h>
#include <unistd.h>
#include <stdio.h>
#include "Prog.h"
JNIEXPORT void JNICALL Java_Prog_f1
  (JNIEnv *pjni, jobject obj){
	printf("In JNI method!\n");
	memcpy(NULL,NULL,1);
	int a = 1/0;
}

