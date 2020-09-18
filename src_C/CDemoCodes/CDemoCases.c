#include <stdio.h>
#include <string.h>
int main()
{
    int aINT[3];
    int *cINT;
	int  **bINT;
	memset(aINT,0,sizeof(int)*3);
	aINT[0] = 1;
	aINT[1]=2;
	aINT[2] = 3;
	printf("aINT=%d%d%d\n",aINT[0],aINT[1],aINT[2]);
	cINT = aINT;
	printf("cINT=%d%d%d\n",cINT[0],cINT[1],cINT[2]);

	bINT = &cINT;
	printf("bINT=%d%d%d\n",(*bINT)[0],(*bINT)[1],(*bINT)[2]);
	int * piA = (int *)bINT;
	printf("piA=%d%d%d\n",piA[0],piA[1],piA[2]);
	int * piB = (int *)*bINT;
	printf("piB=%d%d%d\n",piB[0],piB[1],piB[2]);

}
