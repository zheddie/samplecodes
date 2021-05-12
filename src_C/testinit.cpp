#include <iostream>
#include <string.h>

int main(int argc, char * argv[]){
	char s[5]="1234";
	char t[20]={0};
	char v[20];
	memset(v,0x0,20);
	printf("s=%s\n",s);
	printf("t=%s\n",t);
	printf("v=%s\n",v);
}
