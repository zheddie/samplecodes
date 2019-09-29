#include <stdio.h>
extern	char **environ;
int main(){
	register char **e;

	for (e = environ; *e != NULL; e++)
        {      
        	(void) puts(*e);
         }

}
