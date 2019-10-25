#include <stdio.h>
int ___strlen(char *);
int main(){
	char * msg = "HelloMillicode";
	int len = ___strlen(msg);
	printf("len(%s)=%d\n",msg,len);
}
