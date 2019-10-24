#include "JavaNativeHelloWorld.h"
#include <stdio.h>
/* Header for class JavaNativeHelloWorld */
 
/*
 * Class:     JavaNativeHelloWorld
 * Method:    Nhelloworld
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_JavaNativeHelloWorld_Nhelloworld
  (JNIEnv *, jobject)
{
    printf("Hello, JNI...");
}
 
int main()
{
    return(0);
}

