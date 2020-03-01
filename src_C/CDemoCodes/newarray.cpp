#include <iostream>
using namespace std;
class A{
	public:
	//A(){cout<<"in construction!"<<endl;}
	A(int x){cout<<"x="<<x<<endl;}
};
int main(){
	A pa[10];
	//A pb[10](8);	***WRONG, can not initialize array in this way.*
	int pp[10];
	int *p ;
	p = new int[10];
	for(int i =0;i<10;i++){
		p[i] = i ;
		cout<<"p["<<i<<"]"<< p[i]<<endl;
		cout<<"pp["<<i<<"]"<< pp[i]<<endl;
	}
	delete [] p;
	A *ps;
	ps = new A[10];
	ps[0] = A(1);
	A a = A(3);
	a = A(4);
	int t = int(5);


	return(0);
}
