#include <iostream>
using namespace std;
template<typename func_t>
int callop3(int y,func_t op){
	op();
	return(y);
}
int main(){
	int x=1;
	cout<<"in main,&x="<<&x<<endl;
	auto qqq = [=]{
		cout<<"in qqq,x:"<<x<<endl;
		cout<<"in qqq,&x="<<&x<<endl;
	};
	qqq();
	int rt = callop3(x,[=]{
		cout<<"in lmb,x:"<<x<<endl;
		cout<<"in lmb,&x="<<&x<<endl;
	});
	cout << "rt="<<rt<<endl;
	return(0);
}
