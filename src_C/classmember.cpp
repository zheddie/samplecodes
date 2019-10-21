#include <stdio.h>
#include <stdlib.h>
class People{
private:
	int f1;
	int f2;
protected:
	int f3;
	int f4;
public:
	int f5;
	People():f1(1),f2(2),f3(3),f4(4),f5(5){
		printf("People1 in construction!\n");		
	}
	People(int a,int b,int c , int d , int e):f1(a),f2(b),f3(c),f4(d),f5(e){
		printf("People2 in construction!\n");		
	}
	int getf1(){return f1;}
	int getf2(){return f2;}
	int getf3(){return f3;}
	int getf4(){return f4;}

};
class Man:public People{
public:
	int f6;
	Man():People(0,0,0,0,0),f6(6){
		printf("Man in construction!\n");		
	}
};
int main(){
	People *p = new People();
	Man *m = new Man();
	printf("sizeof(people)=%lu,sizeof(man)=%lu,sizeof(int)=%lu\n",sizeof(People),sizeof(Man),sizeof(int));
	printf("f1:%d,f2:%d,f3:%d,f4:%d,f5:%d,f6:%d\n",m->getf1(),m->getf2(),m->getf3(),m->getf4(),m->f5,m->f6);
	return(0);
}
