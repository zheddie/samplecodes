#include <iostream>
using namespace std;
class A{
	public:
	//A(){cout<<"in construction!"<<endl;}
	A(int x){cout<<"x="<<x<<endl;}
};
int main(){
	A pa[2]={A(8),A(2)};
	A *p = new A[3];
	return(0);
}
