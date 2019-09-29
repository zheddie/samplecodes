#include <iostream>
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
};
int main(){
	People s;
	People * sp = new People("zhanggan",40);
	cout << "sizeof(int)="<<sizeof(int)<<",sizeof(People)="<<sizeof(People)<<",sizeof(s)="<<sizeof(s)<<",sizeof(*sp)="<<sizeof(*sp)<<endl;
	cout<< "&s="<<&s<<",sp="<<sp<<endl;
	cout<<"sp->name="<<sp->getName()<<",sp->age="<<sp->getage()<<endl;
	cout<<"&(sp->age)="<<&(sp->age)<<",&(sp->name)="<<&(sp->name)<<endl;
}