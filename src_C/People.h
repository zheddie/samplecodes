/*
 * People.h
 *
 *  Created on: Aug 29, 2016
 *      Author: zhanggan
 */

#ifndef PEOPLE_H_
#define PEOPLE_H_
#include "zgHeap.h"
#include <cstdio>


class People {
public:
	People();
	virtual ~People();
	virtual Run();
	int getAge();
	int setAge(int);
	void setName(char *s);
	char * getName(){return name;};
//	void * operator new( size_t numBytes, void * heap){
//		    printf("People.new is called\n");
//			void * p = ((zgHeap *)heap)->allocate(numBytes);
//			return p;
//	}
private:
	int age;
	char name[20];
};
#endif /* PEOPLE_H_ */
