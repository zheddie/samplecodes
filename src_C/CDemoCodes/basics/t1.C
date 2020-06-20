#include <stdio.h>
#include <string.h>
class T1{
private:
	int size;
public:
	T1(int s){printf("In T1 constructor,s=%d\n",s);size = s;}
	void * operator new(size_t size,void *storage){printf("In new(int,void*)storage=%s\n",(char *)storage);return storage;};
};

main()
{
	void * pVoid = new char[200];
	strcpy((char *)pVoid,"World!");
	T1 * pT1 = new(pVoid) T1(10);
	strcpy((char *)pVoid,"Hello");
	printf("pVoid.add=%p,pVoid=%s,pT1=%p,pT1=%s\n",
		pVoid,(char *)pVoid,pT1,(char *)pT1);
}
