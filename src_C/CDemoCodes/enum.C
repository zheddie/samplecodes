#include <cstdio>
#include <cstring>
class People{
private:
	int age;
	char name[8];
	int sex;
	enum{
	   boy=0,
	   girl,
	   both
	};
	int sex2;
public:
	int getAge(){return age;}
        void setAge(int a){ age=a;}
	People(int x):age(x){
		strcpy(name,"xname");
		sex = girl;
		sex2=both;
	}
};
int main(){
	People *s = new People(30);
	printf("s(%p)=%d,sizeof(s)=%lu\n",s,s->getAge(),sizeof(*s));
}
