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
#include <stdlib.h>

/*
 * Class:     PaseExample1
 * Method:    getStringNative
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_PaseExample1_getStringNative(JNIEnv* env, jobject obj) {
   char* methodName = "getStringCallback";
   char* methodSig = "()Ljava/lang/String;";
   jclass clazz = (*env)->GetObjectClass(env, obj); 
   jmethodID methodID = (*env)->GetMethodID(env, clazz, methodName, methodSig);
   return (*env)->CallObjectMethod(env, obj, methodID); 
}