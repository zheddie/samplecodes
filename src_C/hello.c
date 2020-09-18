#include <stdio.h>
float getage()
{
	int x = 8;
	char s = 's';
	float y = 8.942;
	return(s);
}

int main(){
	float age;
	age = getage();
	printf("Hello age:%8.2f,%p\n",age,&age);
	return(0);
}
