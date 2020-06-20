#include <iostream>
#include <CPoint.h>
using namespace std;
CPoint exchangePoint(CPoint s);
void fun(int x = 0,int y  = 0 , int z= 0){
	cout << "x+y+z="<<x+y+z<<endl;
}
int main(){
	CPoint x(8,20);
	x.printPoint();
	x.move(4,4);
	x.printPoint();
	CPoint y(x);
	y.printPoint();
	CPoint z = exchangePoint(y);
	y.printPoint();
	z.printPoint();
	CPoint s(0,0);
	s.printPoint();
	s.copyPoint(z);
	s.printPoint();

	return(0);
}
CPoint exchangePoint(CPoint s){
	cout <<"***********************"<<endl;
	s.printPoint();
	s.setPoint(200,100);
	s.printPoint();
	cout <<"***********************"<<endl;
	return s;
}
