#include <cstdio>
#include <cstring>
#include <cstdlib>
void func1(char * p){
	printf("string(%d):%s\n",sizeof(p),p);
}
int main(){
	int rt = 0;
	void (*fp)(char *)  = func1; 
	printf("rt(%d)=%d,fp(%d)=%p\n",sizeof(int),rt,sizeof(fp),fp);	
	char *s=NULL;
	s = (char *)malloc(100);
	memset(s,0,100);
	strcpy(s, "HelloIBMi");
	func1((char *)s);
	return(0);
}
