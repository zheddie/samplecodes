#include <stdio.h>

int main(int argc, char * argv[]){
	int i=0;
	short s = 0;
	long l = 0;
	long long ll = 0;
	float f = 0;
	double d = 0;
	long double ld= 0;
	printf("int:%d,short:%d,long:%d,longlong:%d,float:%d,double:%d,longdouble:%d\n",
			sizeof(int),sizeof(short),sizeof(long),sizeof(long long),sizeof(float),sizeof(double),sizeof(long double));
	return(0);
}
