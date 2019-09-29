#include <iostream>
thread_local int pi=8;
int main(){
	std::cout<<"pi="<<pi<<std::endl;
	return(0);
}
