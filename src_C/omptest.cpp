/*
 * omptest.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: zhanggan
 */

#include <iostream>
#include <ctime>
#include <cstdlib>
//#include <omp.h>
#define MAXINDEX 1000000000
using namespace std;
int main(){
	time_t *tt;
	tt = (time_t *)malloc(sizeof(time_t));
	time(tt);
	int *a,*b,*c;
	a = (int *)malloc(sizeof(int)*MAXINDEX);
	b = (int *)malloc(sizeof(int)*MAXINDEX);
#pragma omp parallel for
	for (int j=0;j<MAXINDEX;j++){
		time(tt);
		a[j] = *tt % 1000;
		b[j] = *tt %100;
	}


	c= (int *)malloc(sizeof(int)*MAXINDEX);

#pragma omp parallel for
	for(int i =0;i<MAXINDEX;i++){
		c[i] = (a[i]+243)*(b[i]+1234);
	}
	int sum = 0;
#pragma omp parallel for

	for(int k=0;k<MAXINDEX;k++){
		sum += c[k];
	}
	cout<<"Count Done!Sum="<<sum<<endl;

}
