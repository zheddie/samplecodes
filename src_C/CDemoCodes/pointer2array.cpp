#include <iostream>
using namespace std;
int *p[3];
int (*pp)[3];
int main(){
	int s[3];
	for(int i =0; i<3;i++){
		s[i]=5;
		p[i] =(int *) malloc(sizeof(int));
		*p[i] = 10;
	}
	pp = &s;
	cout <<"sizeof(p)="<<sizeof(p)<<",sizeof(pp)="<<sizeof(pp)<<endl;
	return(0);
}
