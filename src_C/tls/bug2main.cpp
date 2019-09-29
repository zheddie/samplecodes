#include <iostream>
extern thread_local int pi;
int main(){
	std::cout<<"pi="<<pi<<std::endl;
	return(0);
}
