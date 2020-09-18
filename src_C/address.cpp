#include <iostream>
#include <string.h>
using namespace std;
class People{
public:
	int age=0;
	char * name=NULL;

	People():age(10),name(NULL){
	}
	People(char *n):age(10){
		if(name != NULL){
			free(name);
			name = NULL;
		}
		name = (char *)malloc(strlen(n));
		strcpy(name,n);
	}
	People(string n,int a){
		if(name != NULL){
			free(name);
			name = NULL;
		}
		name = (char *)malloc(n.length());
		strcpy(name,n.c_str());
		age = a;
	}
	int getage(){return age;}
	char * getName(){return name;}
	~People(){
		if(name!=NULL){
			free(name);
		}
	}
	virtual bool canrun(){
		return true;
	}
};
int main(){
	People s;
	People * sp = new People("zhanggan",40);
	cout << "sizeof(int)="<<sizeof(int)<<",sizeof(void *)="<<sizeof(void *)<<",sizeof(People)="<<sizeof(People)<<",sizeof(s)="<<sizeof(s)<<",sizeof(*sp)="<<sizeof(*sp)<<endl;
	cout<< "&s="<<&s<<",sp="<<static_cast<void *>(sp)<<endl;
	cout<<"sp->name="<<sp->getName()<<",sp->age="<<sp->getage()<<endl;
	cout<<"&(sp->age)="<<static_cast<void *>(&(sp->age))<<",&(sp->name)="<<static_cast<void *>(&(sp->name))<<endl;
	/*
 * 	cout<<"&(sp->getName())="<<static_cast<void *>(&(sp->getName))<<endl;
	cout<<"&(sp->getage())="<<static_cast<void *>(&(sp->getage))<<endl;
	void * p =static_cast<void *>(&(sp->age));
	void *pf = (void *) (p-8);
	cout <<"fpointer="<<<static_cast<void *>(*pf)<<endl;
*/
}
