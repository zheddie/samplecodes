#include <iostream>
using namespace std;
class Singleton{
	char * p;
	int a;
	static char *ps;
	public:
		static Singleton& getInstance(){
			static Singleton instance;
			return instance;
		}
		void printTest(){
			cout<<"do something"<<endl;
		}
	private:
		Singleton(){p=NULL;}//防止外部调用构造创建对象
		Singleton(Singleton const &singleton);//阻止拷贝创建对象
		Singleton& operator=(Singleton const &singleton);//阻止赋值对象
};
int main()
{
	Singleton &a=Singleton::getInstance();
	Singleton *b=&(Singleton::getInstance());
	cout<<"Singleton size:"<<sizeof(Singleton)<<","<<sizeof(*b)<<endl;
	a.printTest();
	b->printTest();
	return 0;

}
