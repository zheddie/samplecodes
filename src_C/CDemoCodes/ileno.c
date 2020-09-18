#include <stdio.h>
#include <as400_protos.h>
int main(){
	int ILENO = 108;
	int PASENO = _CVTERRNO(ILENO);
	printf("ILENO=%d,PASENO=%d\n",ILENO,PASENO);
}

