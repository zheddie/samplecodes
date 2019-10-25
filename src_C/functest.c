#include <stdio.h>
int hello(int);
int hello(char *);
int hello(int x){
	printf("intx=%d\n",x);
	return(0);
}
int hello(char *x){
	printf("char*x=%s\n",x);
	return(0);
}
