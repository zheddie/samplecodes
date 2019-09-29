#include <iostream> 
using namespace std; 
class A { 
public :  
	static void test1() { cout<<"A::test1 called"<<endl; };
}; 
void test2(){
	cout<<"test2 called"<<endl;
}
typedef void (*PFunc)(); 
int main() 
{  
	PFunc f = &(A::test1);   
	(*f)();  
	f = test2;
	(*f)();
 	return 0;
}