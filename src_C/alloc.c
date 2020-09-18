#include <stdio.h>
#include <stdlib.h>

static int depth=0;

int myfunc(){
	int * p = malloc(sizeof(int));
	*p = depth;
	printf("myfunc:%d,&p=%p,p=%p,*p=%d\n",depth,&p,p,*p);
	depth+=1;
	if(depth<10){
		myfunc();
	}
	return(0);
}

int main(){
	myfunc();
	return(0);
}
