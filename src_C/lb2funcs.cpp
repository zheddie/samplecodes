#include <iostream>
using namespace std;
template<typename func_t>
static inline float callop3(float x,func_t op){
	cout<<"in callop3(),x="<<x<<endl;
	op();
	return(x);
}
template<typename func_t>
static inline float callop2(float x,func_t op){
	float s  = callop3(x,op);
	return(s);
}
template<typename func_t>
float callop(float x,func_t op){
	float s = callop2(x,op);
	return(s);
}
float entry(){
	float x=1.;
	cout<<"in entry(),x="<<x<<",&x="<<&x<<",sizeof(x)="<<sizeof(x)<<endl;
	auto qqq = [=]{
		cout<<"in qqq,x:"<<x<<endl;
		cout<<"in qqq,&x="<<&x<<endl;
	};
	x=77;
	//callop(x,qqq);
	float y = callop(x,
		[=] {
		cout<<"in lbm,x:"<<x<<endl;
		cout<<"in lbm,&x="<<&x<<endl;
		}
	);
	return(y);
}
