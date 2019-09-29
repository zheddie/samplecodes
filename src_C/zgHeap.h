/*
 * zgHeap.h
 *
 *  Created on: Nov 25, 2016
 *      Author: zhanggan
 */

#ifndef ZGHEAP_H_
#define ZGHEAP_H_
#include <cstddef>

class zgHeap {
    //friend void * operator new(size_t  , zgHeap *);
public:
	zgHeap();

	void initialize();
	void * getheap(){return heap;};
	virtual ~zgHeap();
	void * allocate(int);
private:

	void * heap;
	void *free;
	int  allocatedsize;
};

//typedef class zgHeap HeapCls;

#endif /* ZGHEAP_H_ */
