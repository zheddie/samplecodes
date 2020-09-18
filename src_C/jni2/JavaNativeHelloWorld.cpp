#include "JavaNativeHelloWorld.h"
#include <stdio.h>
#include <stdlib.h>
/* Header for class JavaNativeHelloWorld */
 
static int depth=0;
int myfunc();
/*
 * Class:     JavaNativeHelloWorld
 * Method:    Nhelloworld
 * Signature: ()V
 */

JNIEXPORT void JNICALL Java_JavaNativeHelloWorld_Nhelloworld(JNIEnv *, jobject)
{
    printf("Hello, JNI...\n");
    myfunc();
}
 
int main()
{
    myfunc();
    return(0);
}

int myfunc(){
	int * p = (int *)malloc(sizeof(int));
	* p = depth;
	printf("myfunc:%d,&p=%p,p=%p,*p=%d\n",depth,&p,p,*p);
	depth+=1;
	if(depth < 10){
		myfunc();
	}
	return(0);
}
