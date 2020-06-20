#include <iostream>
using namespace std;
class A{
	public:
	A(int x){a=x;}
	void setA(int x) const{cout<<"a=x can not work"<<endl;}
	void print() const {cout<<"a="<<a<<endl;}
	private:
	int a;
	};
const A ia(8);
int main(){
	ia.print();
	ia.setA(9);
	ia.print();
	A ib(3);
	ib.print();
	ib.setA(10);
	ib.print();
	return(0);
}
