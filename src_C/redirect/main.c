#include <stdio.h>
int main(){
	fprintf(stdout,"Hello stdout\n");
	fprintf(stderr,"Hello stderr\n");
	fflush(stdout);fflush(stderr);
	return(0);
}
