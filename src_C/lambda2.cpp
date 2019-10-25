#include <iostream>
float entry();
using namespace std;
int main(){
	float y = entry();
	cout<<"in main, return from entry:="<<y<<endl;
	return(0);
}
