#include <iostream>
using namespace std;
class A
{
public:
	A(int i){_a=i;}
	static void main(){cout<<"A::main() called."<<endl;};
	void f2(){cout<<"A::f2() called._a="<<_a<<endl;}
	virtual void f(){cout<<"A::f() called._a="<<_a<<endl;}
private:
	int _a;
};
class B: public A
{
public:
	B(int i):A(i*i){_b=i;}
	virtual void f(){cout << "B::f() called,_b="<<_b<<endl;}
private:
	int _b;
};
int main()
{
	cout <<"Try dynamic combine!"<<endl;
	B b(5);
	A &r = b;
	void (*spf)()=&(A::main);//类静态成员函数指针，声明跟普通函数指针一致,但取函数地址需要地址符&。普通函数直接写函数名。
	(*spf)();	//使用也同普通函数指针
	void (A::*pf)()=NULL;
	void (A::*pf2)()=NULL;
	pf=&A::f;	//对象函数指针，声明时需要加类修饰符(A::*pf),取函数地址需要地址符&,并且此处不能使用括号括起来。
	pf2 = &A::f2;//DEBUG: pf,pf2,并不是实际意义上的指针，似乎是一个以某种形式指向的函数表中的某个位置。
	(r.*pf)();//对象函数指针，使用时，需要跟在某个对象后,并且用括号括起来:(r.*pf)
	(r.*pf2)();
	cout <<"Try static combine!"<<endl;
	A a(9);
	a.f();
	return(0);
}