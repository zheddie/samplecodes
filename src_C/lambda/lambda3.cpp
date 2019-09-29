#include <iostream>
using namespace std;
template<typename func_t>
float callop3(float y,func_t op){
	float z=3.5;
	op();
	return(z);
}
int main(){
	float x=1.0;
	cout<<"in main,&x="<<&x<<endl;
	auto qqq = [=]{
		cout<<"in qqq,x:"<<x<<endl;
		cout<<"in qqq,&x="<<&x<<endl;

	};
	qqq();
	float rt = callop3(x,[=]{
		cout<<"in lmb,x:"<<x<<endl;
		cout<<"in lmb,&x="<<&x<<endl;
	});
	cout << "rt="<<rt<<endl;
	return(0);
}
