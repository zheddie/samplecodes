/*
 * gettime.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: zhanggan
 */

#include <iostream>
#include <ctime>
using namespace std;
int main(){
	time_t ts;
	for(int i =0;i<10;i++){
	time(&ts);
	cout<<"Time ="<<ts<<endl;
	}
}
