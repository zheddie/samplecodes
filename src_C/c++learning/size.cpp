#include <iostream>
#include <string.h>

class nodata{
public:
	void printString(const char * p){
		std::cout<<"p="<<p<<std::endl;
	}
};
class CT1{
	char p[5];
	class nodata pd;
public:
	void init(){ pd.printString(p) ;}
	CT1(const char *pp){memset(p,0,5);memcpy(p,pp,4);}
};

int main(){
	const char * ss = "HELLO";
	class nodata *p = new nodata();
	std::cout<<"sizeof_p:"<<sizeof(p)<<",sizeof_nodata:"<<sizeof(class nodata)<<std::endl;
	p->printString(ss);
	class CT1 *pct = new CT1(ss);
	pct->init();
	std::cout<<"sizeof_CT1:"<<sizeof(class CT1)<<std::endl;
}
