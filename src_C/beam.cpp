#include <iostream>
using namespace std;
int ps(){
	return(9);
}
int main(){
	int x=8;
	int * p  = &x;
	int (*pf)() = ps;
	x;
	p;
	*p;
	cout<<"x:"<<x<<endl;
	x=pf();
	cout<<"x:"<<x<<endl;
	return(0);
}
