#include <iostream>
#define SS(name) c##name
char crr[]="hello";
#define ST(name) &&c##name
main(){
	int cna = 8;
	std::cout<<SS(na)<<std::endl;	
	std::cout<<ST(rr)<<std::endl;	
}
