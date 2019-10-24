/*
 * nativeone.c
 *
 *  Created on: Sep 11, 2014
 *      Author: zhanggan
 */

#include <stdio.h>
#include "JNITest.h"
JNIEXPORT void JNICALL
Java_JNITest_nativeOne
(JNIEnv* env, jobject thisObj)
{
printf("Hello JNI World\n");
}
