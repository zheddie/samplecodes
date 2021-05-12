/*
 *

 * This native method implements the getStringNative method of class
 * PaseExample1. It uses the JNI function CallObjectMethod to call 
 * back to the getStringCallback method of class PaseExample1.
 *
 * Compile this code in AIX or PASE for i to create module 'libPaseExample1.so'.
 *
 */

#include "callILENative.h"
//#include "j9nonbuilder.h"
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


/*	
 * Class:     PaseExample1
 * Method:    getStringNative
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_callILENative_printString(JNIEnv* env, jobject obj) {
	printf("in Java_callILENative_printString\n");
}
