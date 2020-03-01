/*
 * People.cpp

 *
 *  Created on: Mar 4, 2016
 *      Author: zhanggan
 */

#include "People.h"
namespace std{
template <typename H> People<H>::People() {
	// TODO Auto-generated constructor stub
	memset(Name,0,10);
	age = 0;
}

template <typename H> People<H>::~People() {
	// TODO Auto-generated destructor stub
}
template <typename H> char * People<H>::getName(){
	return Name;
}
template <typename H> int People<H>::getAge(){
	return age;
}
template <typename H> void People<H>::setAge(int a){
	age = a;
}
template <typename H> void People<H>::setName(char * n){
	strncpy(Name,n,10);
}

template <typename H> int People<H>::getHsize(){
	return sizeof(H);
}

}
