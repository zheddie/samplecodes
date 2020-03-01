/*
 * DemoCases.cpp
 *
 *  Created on: Mar 5, 2016
 *      Author: zhanggan
 */

#include <iostream>
#include <stdio.h>
using namespace std;
void testUnion(){
	///////////////////union////////////////
	union                               // Overlapping storage for allocated
	{
	  char *xBuf;
	  char xLocalBuf[20];
	};
	//xBuf = xLocalBuf;
	memset(xLocalBuf,0,20);
	//strcpy(xLocalBuf,"In stack");
	printf("xBuf=\t\t%p,\nxLocalBuf=\t%p\n&xBuf=\t\t%p,\n&xLocalBuf=\t%p\n,xBuf.str=\t\t%s,\nxLocalBuf.str=\t%s\n",xBuf,xLocalBuf,&xBuf,&xLocalBuf,xBuf,xLocalBuf);
	printf("====================================================\n");
	xBuf = new char[30];
	strcpy(xBuf,"In Heap");
	printf("xBuf=\t\t%p,\nxLocalBuf=\t%p\n&xBuf=\t\t%p,\n&xLocalBuf=\t%p\n,xBuf.str=\t\t%s,\nxLocalBuf.str=\t%s\n",xBuf,xLocalBuf,&xBuf,&xLocalBuf,xBuf,xLocalBuf);

/*
 * bogon:src zhanggan$ g++ -m32 -o demo DemoCases.cpp
bogon:src zhanggan$ ./demo
xBuf=		0x8fe92094,
xLocalBuf=	0xbff8dbd0
&xBuf=		0xbff8dbd0,
&xLocalBuf=	0xbff8dbd0
====================================================
xBuf=		0x7b630160,
xLocalBuf=	0xbff8dbd0
&xBuf=		0xbff8dbd0,
&xLocalBuf=	0xbff8dbd0
 *
 */
}
void testClass(){
	class  A{
	public:
		A(int a){ age = a;};
		~A(){};
		int getAge(){return age;};
		void setAge(int a){ age = a;};
		int age;
		union{
		char *Name;
		char localName[15];
		};
		int sp;
	};
	A a(8);
	cout << "a.Age="<<a.getAge()<<endl;
	cout << "sizeof(A)=" <<sizeof(A)<<endl;
	/*
	 * a.Age=8
	 * sizeof(A)=24
	 */
	class B{
	public:
		B(int b){a = b;};
		~B(){};
		const int operator==(const B &c){
			int b = c.getA();

			return (a==b);
		};
		int getA() const{
			return a;
		};
	private:
		int a;
	};
	B bV1(8);
	B *bV2 = new B(8);
	B *bV3 = new B(4);
	if(bV1==*bV2){
		cout <<"bV1==bV2"<<endl;
	}else{
		cout <<"bV1!=bV2"<<endl;
	}
	if(bV1==*bV3){
		cout <<"bV1==bV3"<<endl;
	}else{
		cout <<"bV1!=bV3"<<endl;
	}
}
void testsizeof(){
	int *p ;
	p = (int *)malloc(sizeof(int)*8);
	cout <<"sizeof(int)="<<sizeof(int)<<endl;
	cout <<"sizeof(p)="<<sizeof(p)<<endl;
	free (p);

/*
 * sizeof(int)=4
sizeof(p)=4          ====>Here ,the size of pointer is always 4.
 */
}

void testmemcpy(){
	char * str = NULL;
	char * p =NULL;
	str = (char *)malloc ( 10);
	strcpy(str,"abcdefghij");
	cout<<"str="<<str<<endl;
	strcpy(str,"abcdefghi");
	p = str+3;
	cout << "str="<<str<<",p="<<p<<endl;
	strcpy(str,p);
	cout << "str="<<str<<",p="<<p<<endl;

}
typedef char Name_t[10];
typedef int  INT_t[3];
void copyName(Name_t *p){
	Name_t aName;
	Name_t * bName=NULL;
	memcpy(aName,*p,sizeof(Name_t));
	bName = p;
	cout <<"aName="<<aName<<"*"<<endl<<"bName="<<*bName<<"*"<<endl;

	char * pC = (char *)p;         /*Why this works!!!???, p should be a (char **):NO, it's a pointer to a char array.*/
	char * pD = (char *)*p;
	char *pD1 = (char *)aName;     /*字符数组（＊p）或者aName，本身就跟字符指针可以通用的*/
	cout <<"pC="<<pC<<"*"<<endl;
	cout <<"pD="<<pD<<"*"<<endl;

	cout <<"pD1="<<pD1<<"*"<<endl;

}
void testtypedef(){
	Name_t cName;
	Name_t *dName;
	memset(cName,0x0,sizeof(Name_t));
	memset(cName,0x20,sizeof(Name_t)-1);
	cout <<"cName="<<cName<<"*"<<"&cName="<<&cName<<endl;
	copyName(&cName);
    dName = &cName;
    char *pE = (char *)dName;
    cout <<"pE="<<pE<<"*"<<endl;
    char *pF = (char *)*dName;
    cout <<"pF="<<pF<<"*"<<endl;
    char *pG = (char *)cName;
    cout <<"pG="<<pG<<"*"<<endl;
    char *pH = (char *)&cName;
    cout <<"pH="<<pH<<"*"<<endl;
    /*************for int***************/
    INT_t aINT;
    INT_t *bINT;
    memset(aINT,0x0,sizeof(INT_t));
    aINT[0] = 1;
    aINT[1]=2;
    aINT[2] = 3;
    cout<<"aINT="<<aINT[0]<<aINT[1]<<aINT[2]<<endl;
    bINT = &aINT;
    cout<<"bINT="<<(*bINT)[0]<<(*bINT)[1]<<(*bINT)[2]<<endl;
    int * piA = (int *)bINT;
    cout<<"piA="<<piA[0]<<piA[1]<<piA[2]<<endl;
    int * piB = (int *)*bINT;
    cout<<"piB="<<piB[0]<<piB[1]<<piB[2]<<endl;

    /*
    int aINT[3];
    int *cINT;
	int  **bINT;
	memset(aINT,0x0,sizeof(INT_t));
	aINT[0] = 1;
	aINT[1]=2;
	aINT[2] = 3;
	cout<<"aINT="<<aINT[0]<<aINT[1]<<aINT[2]<<endl;
	cINT=aINT;
	cout<<"cINT="<<cINT[0]<<cINT[1]<<cINT[2]<<endl;
	bINT = (int**)&cINT;
	cout<<"bINT="<<(*bINT)[0]<<(*bINT)[1]<<(*bINT)[2]<<endl;
	int * piA = (int *)bINT;
	cout<<"piA="<<piA[0]<<piA[1]<<piA[2]<<endl;
	int * piB = (int *)*bINT;
	cout<<"piB="<<piB[0]<<piB[1]<<piB[2]<<endl;

*/

}
void copyNameStr(char **p){
	char * pC = (char *)*p;
	char *pD = (char * )p;
	cout <<"pC="<<pC<<"*"<<endl;
	cout <<"pD="<<pD<<"*"<<endl;
}
void teststring(){

	char *p = (char *)malloc(10);
	memset(p,0x0,10);
	strcpy(p,"hello");
	char **ps = &p;
	cout<<"p="<<p<<",ps="<<ps<<",*ps="<<*ps<<endl;
	char *st = (char *)ps;
	cout<<"st="<<st<<endl;
	copyNameStr(&p);
	/**/
	char d [10];
	memset(d,0x0,10);
	strcpy(d,"hello");
	cout<<"d="<<d<<endl;     //其实是将d认为是一个字符串来打印
	cout<<"&d="<<&d<<endl;

	char *d1 =(char *)d;			//把d这个变量（或者认为一个stack中的地址）转换成(char *)
	char *d2 = (char *)&d;         //Here it should work. &d does NOT mean it's (char **). 这个只是说明把d的地址转换成（char ＊）的地址去解释
	cout <<"d1="<<d1<<",d2="<<d2<<endl;
}
template<typename F>
class TC1{
public:
	TC1(){};
	TC1(F f){_iVal = f;};
	~TC1(){};

	F doDouble(){return _iVal * 2;};
private:
	F _iVal;
};
void testTemplate(){

	class TC1<int> mytc1(8);
	cout<<"mytc1.doDouble="<<mytc1.doDouble()<<endl;
	class TC1<char> mytc2('8');
	cout<<"mytc2.doDouble="<<mytc2.doDouble()<<endl;

}
void testAbstractClass(){
	class Polygon {
	  protected:
	    int width, height;
	  public:
	    virtual ~Polygon(){}
	    void set_values (int a, int b)
	      { width=a; height=b; }
	    virtual int area (void) =0;
	    virtual int operator==(Polygon &) =0;
	};

	class Rectangle: public Polygon {
	  public:
	    int area (void)
	      { return (width * height); }
	    int operator==(Rectangle &b){
	    		return area() == b.area();
	    }
	    int operator==(Polygon &b){
	    	    		return area() == b.area();
	    	    }
	};

	class Triangle: public Polygon {
	  public:
	    int area (void)
	      { return (width * height / 2); }
	    int operator==(Rectangle &b){
	    		return area() == b.area();
	    }
	};

	Rectangle * p = new Rectangle();
	p->set_values(10,20);
	cout<<"p->area=" <<p->area()<<endl;

}

int main() {
	//cout << "!!!zg.Hello World!!!" << endl; // prints !!!Hello World!!!
	cout << "---------testUnion()"<<endl;
	testUnion();
	cout << "---------testClass()"<<endl;
	testClass();
	cout << "---------testsizeof()"<<endl;
	testsizeof();
	cout << "---------testmemcpy()"<<endl;
	testmemcpy();
	cout << "---------testtypedef()"<<endl;
	testtypedef();
	cout << "---------teststring()"<<endl;
	teststring();
	cout << "---------testTemplate()"<<endl;
	testTemplate();
	cout << "---------testAbstractClass()"<<endl;
	testAbstractClass();
	return 0;
}

