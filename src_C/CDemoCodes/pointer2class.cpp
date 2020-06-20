#include <iostream>
using namespace std;
class B{
	public:
	B(){a = 0;b=10;c=80;p=&c;}
	int a,b;
	int *p;
	static int c;
	void printall();
};
int B::*pa = & B::a;
//int B::*pc = & B::c;
int B::c = 200;
void B::printall(){
	cout <<"a="<<a<<",b="<<b<<",c="<<B::c<<",*p ="<<*p<<endl;
}
int main(){
	B ib;
	int B::*pb = &B::b;
	int B:: *pc = &B::a;
	ib.printall();
	ib.p = & ib.a;
	ib.printall();
	int c = 20;
	ib.p = &c;
	ib.printall();
	ib.*pa = 30;
	ib.printall();
	*(ib.p) = 40;
	ib.printall();
	ib.*pb = 90;
	ib.printall();
	pa = &B::b;
	cout <<"pa="<<ib.*pa<<endl;
	return(0);
}
