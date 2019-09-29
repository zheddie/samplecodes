#include <stdlib.h>
#include <stdio.h>
struct s1{
	int i;
	char * p;
	void (* pf1)(void);
	};

struct s{
	const struct s1 t1;
	int i;
	char * p;
	void (* pf1)(void);
	void (* pf2)(void);
	};
int main(){
	struct s t;
	printf("sizeof(t)=%lu\n",sizeof(t));

}
