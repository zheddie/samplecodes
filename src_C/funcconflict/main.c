#include <stdio.h>
extern int hello(int);
extern int hello(char *);
int main(){
	int ix  = 10;
	char sx[8] = "stringX";
	hello(ix);
	hello(sx);
}
