#include <iostream>
class People{
private:
	static const int count = 8;
	static const char * race;
public:
	static const char * getRace(){return race;};
	static int getCount(){return count;};
	People(){};
	void init(){
		const char * p = "USA";
		People::race = p;
	}
	const char * init2(int s){
		const char * pp = "JPA";
		return(pp);
	}
};
const char * People::race = "Chinese";

