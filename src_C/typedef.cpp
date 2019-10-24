#include <iostream>
using namespace std;
typedef int * *array;
int main(){
	array a,*b;
	a = new int*[5];
	for(int j = 0 ; j < 5 ; j++){
		a[j] = new int;
	}
	*a[0]=5;
	*a[3]=18;
	b = &a;
	for(int i = 0 ; i < 5 ; i ++ )
	{
		cout <<"*(*b)["<<i<<"]=" << *(*b)[i] <<endl;
	}
	cout <<"*a[0]=" <<*a[0] <<endl;
	cout <<"sizeof(a)" <<sizeof(a) <<endl;
	for(int j = 0 ; j < 5 ; j++){
		delete a[j];
	}
	delete a;
	return 0;
}
