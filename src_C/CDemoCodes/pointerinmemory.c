#include <stdio.h>
#include <unistd.h>
int getn(int);
main(){
	char * p = new char[20];
	char p1[10] = "Hello";
	char * big = new char[1024*1024*64];
	int iA = 20;
	for(int i =0;i<20;i++){
		p[i] = 'a';
	}
	printf("zg.p(%p)=%s,p1(%p)=%s\n",p,p,p1,p1);
	printf("zg.main=%p,big=%p\n",main,big);
	printf("zg.getn=%p,iA:%p\n",getn,&iA);
	int g = getn(iA);
	sleep(5*60);
	delete p,p1,big;
	printf("zg.delete done!\n");
}
int getn(int a){
	char  t[] = "In getn()";
	printf("zg.getn.t=%p\n",t);
	printf("zg.getn,a=%p\n",&a);
	return a*5;
}
