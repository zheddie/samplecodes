/*
 * People.h
 *
 *  Created on: Mar 4, 2016
 *      Author: zhanggan
 */

#ifndef PEOPLE_H_
#define PEOPLE_H_
#include <iostream>
namespace std{
template <typename H>
class People {
public:

	People();

	virtual ~People();
	char * getName();
	int getAge();
	void setAge(int a);
	void setName(char * n);

	int getHsize();
private:
	char Name[10];
	int age;
	H header;
};
}
#endif /* PEOPLE_H_ */
