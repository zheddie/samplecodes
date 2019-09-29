#include <stdio.h>

int statMode(const char *, int *);
int main(int args, char * argv[]){
	int mode  = 0;
	statMode(argv[1],&mode);
	//int fmt = mode & S_IFMT;
    printf("mode=%X\n",mode);
	return(0);
}
