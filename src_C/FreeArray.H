/*
 * FreeArray.H
 *
 *  Created on: Jun 14, 2017
 *      Author: zhanggan
 */

#ifndef FREEARRAY_H_
#define FREEARRAY_H_

class FreeArray {
public:
	FreeArray(){
		age = 18;
		strcpy(name,"dada");
	}
	virtual ~FreeArray();
private:
	int age;
	char name[20];
};

#endif /* FREEARRAY_H_ */
