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
class People1{
private:
	int age;
	char name[8];
	int sex;
	int sex2;
	union{
		int l1;
		int l2;
		int l3;
	};
public:
	int getAge(){return age;}
        void setAge(int a){ age=a;}
	People1(int x):age(x){
		strcpy(name,"xname");
		sex = 2;
		sex2=3;
	}
};
int main(){
	People *s = new People(30);
	People1 *s1 = new People1(31);
	printf("s(%p)=%d,sizeof(s)=%lu\n",s,s->getAge(),sizeof(*s));
	printf("s1(%p)=%d,sizeof(s1)=%lu\n",s1,s1->getAge(),sizeof(*s1));
}
