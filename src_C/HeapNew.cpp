/*
 * HeapNew.cpp
 *
 *  Created on: Aug 29, 2016
 *      Author: zhanggan
 */
#include <stdio.h>
#include <memory.h>
#include <iostream>
#include "People.h"
#define HEAPSIZE 1024
using namespace  std;

int main(int agc, char ** argv){
	void * heap = (void *)malloc(HEAPSIZE);
	memset(heap,0,HEAPSIZE);
	printf("zg.&heap=%p\n",heap);
	People * s1 = new People();
	printf("zg.&s1(from system)=%p\n",s1);
	People * s2 = new (heap)People();
	printf("zg.&s2(from heap)=%p\n",s2);
	People * s3 = new (heap)People();
	printf("zg.&s3(from heap)=%p\n",s3);


}


