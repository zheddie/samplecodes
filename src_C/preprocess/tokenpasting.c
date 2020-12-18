#include <stdio.h> 
#define H xx
#define concat(a, b) &&a##b 
int main(void) 
{
	int t = 0; 
xx:
	printf("in xx\n");
	if(t!=0)
		goto xy;
    	printf("%p\n", concat(H,H)); 
	t=1;
	goto xx;
xy:    
	return 0; 
}
