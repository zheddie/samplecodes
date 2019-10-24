#include <stdlib.h>
#include <stdio.h>
class CA{
public:
	CA(int);
	int getvalue();
	void setvalue(int);
private:
	int age;
};
CA::CA(int a){
	age = a;
	}
int CA::getvalue(){
	return age;
}
void CA::setvalue(int s){
	age = s;
}
int main(){
	class CA *my1 = new CA(10);
	CA *my2 = new CA(8);
	CA my3(6);
	int t(12);
	printf("zg.my1=%d,my2=%d,my3=%d\n",my1->getvalue(),my2->getvalue(),my3.getvalue());
	printf("zg.t=%d\n",t);
	return(0);
}
