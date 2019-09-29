/*
 * zgHeap.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: zhanggan
 */

#include "zgHeap.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#define MAXHEAP 1024*1024
zgHeap::zgHeap() {
	// TODO Auto-generated constructor stub
	printf("zgHeap::zgheap()\n");
	heap = NULL;
	free  = NULL;
	allocatedsize = 0;
}
void zgHeap::initialize(){
	printf("zgHeap::initialize()\n");
	heap = (void *)malloc(MAXHEAP);
	memset(heap,0xFF,MAXHEAP);
	free = heap;
	allocatedsize  = 0;
}
void * zgHeap::allocate(int size){

	void *pos = free;
	free = (void *) ((char *)free+ size);
	allocatedsize +=size;
	if(free > (char *)heap + MAXHEAP){
		pos = heap;
		free = (void *)((char *)heap +size);
		allocatedsize = size;
	}
	return pos;
}
zgHeap::~zgHeap() {
	// TODO Auto-generated destructor stub
}

void * operator new(size_t size,zgHeap* hp){
	printf("friend new is called.\n");
	return ((zgHeap *)hp)->allocate(size);
}
