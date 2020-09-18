#include <iostream>
using namespace std;
class People{
private:
	int age;
public:
	int getAge(){
		return age;
	}
	void setAge(int a){
		age=a;
	}
};
class Man: public People{
private:
	int steps;
public:
	int getSteps(){
		return(steps);
	}
	void setSteps(int s){
		steps = s;
	}
	void run(){
		cout<<"Start run:age="<<getAge()<<endl;
		for(int i =0;i<steps;i++){
			setAge(getAge()+1);
		}
		cout<<"End run:age="<<getAge()<<endl;
	}
};
int main(){
	class Man *one = new Man();
	one->setSteps(8);
	class People *p = one;
	p->setAge(10);
	((class Man *)p)->run();
	return(0);
}