#include <iostream>
#include "People.hpp"
using namespace std;
int main(){
	People one;
	cout <<"count="<<People::getCount()<<",race="<<People::getRace()<<endl;
	one.init();
	one.init2(9);
	cout <<"count="<<People::getCount()<<",race="<<People::getRace()<<endl;
}
