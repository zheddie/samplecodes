/*
 * main.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: zhanggan
 */
#include "People.h"
#include "zgHeap.h"
#include <iostream>
using namespace std;
void *operator new(size_t, zgHeap *) ;
int main(){
	zgHeap * hp = new zgHeap();
	hp->initialize();
//	void * heap = hp->getheap();
	 People * a = new(hp) People();
	a->setName("Zhang");
	a->setAge(30);
	 People * b = new(hp) People();
	b->setName("Wang");
	b->setAge(20);
	//cout<<"Name:"<<a->getName()<<",Age:"<<a->getAge()<<endl;
	printf("hp=%p,heap=%p,a=%p,b=%p,Name=%s,Age=%d\n",hp,hp->getheap(),a,b,a->getName(),a->getAge());
}
