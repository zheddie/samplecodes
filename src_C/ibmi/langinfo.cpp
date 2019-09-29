/*
 * langinfo.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: zhanggan
 */



#include <stdio.h>
#include <langinfo.h>
#include <iostream>
using namespace std;
int main(){
	char *codepage;
	codepage = nl_langinfo(0);
	cout<<"codepage="<<codepage<<endl;
	return 0;
}
