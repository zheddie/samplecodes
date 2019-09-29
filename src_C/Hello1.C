/*
 * Hello1.C
 *
 *  Created on: Jun 9, 2017
 *      Author: zhanggan
 */

#include "Hello1.h"
#include <iostream>
using namespace std;


Hello1::Hello1() {
	// TODO Auto-generated constructor stub
	age = 10;


}

Hello1::~Hello1() {
	// TODO Auto-generated destructor stub
}
int Hello1::getage(){
	return age;
}
struct AA{

	int age;
	AA(int a){ printf("AA construct...with a=%d\n",a);age = a;}
	AA(){ age = 8;printf("AA construct...\n");}

};
struct AA x(int w){
	struct AA *wt = (struct AA *)malloc(sizeof(struct AA));
	wt->age = w;
	return *wt;
};

int Hello1::s = 20;
int main(){
	int * sp[5] = {NULL};
	int * st[5];
	int * ss[5] = {NULL,NULL,NULL,NULL,NULL};
	if(1){
		int j = 12;
		ss[0] = &j;
		ss[1] = &j;
		ss[2] = &j;
		ss[3] = &j;
		ss[4] = &j;
	}
	int so = 9;
	for (int t = 0;t<5;t++){
		cout<<"ss["<<t<<"]="<<*ss[t]<<endl;
	}
	struct AA y,x1(12);


	struct AA z(5);
	struct AA a = x(18);

	printf("y=%d,z=%d,x1=%d,a=%d\n",y.age,z.age,x1.age,a.age);

	Hello1 zh ;
	printf("zh.age=%d,s=%d",zh.getage(),Hello1::s);
}
