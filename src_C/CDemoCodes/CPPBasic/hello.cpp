#include <iostream>
using namespace std;
class MyTest{
private:
	int age ;
	char * name;
public: 
	MyTest(){age = 1;name= new char(20);}
	~MyTest(){ free(name);}
	int setage(int a){age = a;}
	int getage(){return age;}
	int setname(char *s){strcpy(s,name);}
	char * getname(){return(name);}
};
int main(){
	MyTest s;
	cout<<"Hello!"<<endl;
	s.setage(8);
	std::cout<<"Age="<<s.getage()<<std::endl;
	return(0);
	}
