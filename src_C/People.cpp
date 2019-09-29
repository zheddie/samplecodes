/*
 * People.cpp
 *
 *  Created on: Aug 29, 2016
 *      Author: zhanggan
 */

#include "People.h"
#include <string.h>


People::People() {
	// TODO Auto-generated constructor stub
	age = 0;
	printf("People::People()\n");
	memset(name,0,20);
}

People::~People() {
	// TODO Auto-generated destructor stub
}
void People::setName(char * s){
	 memset(name,0,20);
	 memcpy(name,s,19);
}
int People::setAge(int a){
	age  = a;
	return 0;
}
int People::getAge(){
	return age;
}

