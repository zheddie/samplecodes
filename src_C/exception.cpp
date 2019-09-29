#include <iostream>
#include <thread>
void tfun(){
	try{
		throw std::exception();
	}catch(...){
		std::cout<<"Exception catched!\n";
	}
	std::cout<<"in tfun , after exception"<<std::endl;
}
int main(){
	std::thread t1(tfun);
	t1.join();
	std::cout<<"in main after t1.join"<<std::endl;
	return(0);
}
