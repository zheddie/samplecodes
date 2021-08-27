#include <stdio.h>
int main(){
	union {
        float y [3][4];
	int  t[3][4];
	unsigned char s[48];
	} v;
//	float y[3][4];
	printf("&y=%p,sizeof(int)=%lu,sizeof(flaot)=%lu\n",v.y,sizeof(int),sizeof(float));
	for(int i=0;i<3;i++){
		for(int j = 0;j<4;j++){
			v.y[i][j] = (float)(i+1)*(j+1);
			printf("%f(0x%X)",v.y[i][j],v.t[i][j]);
			for(int k=0;k<4;k++){
				printf(".%02X",v.s[i*4*4+j*4+k]);
			}
			printf(" ");
		}
		printf("\n");
	}
	for(int i=0;i<3;i++){
		for(int j = 0;j<4;j++){
			printf("%p ",&v.y[i][j]);
		}
		printf("\n");
	}
}
