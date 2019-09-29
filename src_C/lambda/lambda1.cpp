#include <iostream>
using namespace std;

int main(){
	auto x = 8;
	auto y= 42;
	auto qqq = [=](int a,int b){
		std::cout<<"x="<<x<<",&x="<<&x<<std::endl;
		std::cout<<"y="<<y<<",&y="<<&y<<std::endl;
		std::cout<<"a="<<a<<",&a="<<&a<<std::endl;
		std::cout<<"b="<<b<<",&b="<<&b<<std::endl;
		};
	x=y=77;
	std::cout<<"out of qqq:x="<<x<<",&x="<<&x<<std::endl;
	qqq(x,y);
	std::cout<<"out of qqq:y="<<y<<",&y="<<&y<<std::endl;
	qqq(x,y);
	std::cout<<"finally:y="<<y<<",&y="<<&y<<std::endl;
	return(0);

}
