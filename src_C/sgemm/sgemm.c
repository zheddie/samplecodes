#include <stdio.h>
#include <unistd.h>
#include <cblas.h>
void printMatrix(void *array , int r, int c, char *name){
	float (*p)[r][c] = (float (*)[r][c])array;
	int i=0,j=0;
	for( i =0;i<r;i++){
		printf("%s[%d]:",name,i);
		for(j=0;j<c;j++){
			printf("%f,",(*p)[i][j]);
		}
		printf("\n");
	}

}


int main(){
	int i=0,j=0,k=0;
	float a[2][4] = {{1.0,2.0,2.0,3.0},{2.0,4.0,1.0,2.0}};
	float b[4][1]={{2.0},{1.0},{3.0},{4.0}};
	float c[2][1];
	printMatrix(a,2,4,"a");
	printMatrix(b,4,1,"b");
	/*
	for( i =0;i<2;i++){
		printf("a[%d]:",i);
		for(j=0;j<4;j++){
			printf("%f,",a[i][j]);
		}
		printf("\n");
	}
	for(j=0;j<4;j++){
		printf("b[%d]:",j);
		for(k =0;k<1;k++){
			printf("%f,",b[j][k]);
		}
		printf("\n");
	}
	*/
	for(i = 0;i<2;i++){
	//	printf("c[%d]:",i);
		for(k=0;k<1;k++){
			float tmp = 0.0;
			for(j=0;j<4;j++){
				tmp+= a[i][j]*b[j][k];
			}
			c[i][k]=tmp;
	//		printf("%f,",c[i][k]);
		}
	//	printf("\n");
	}
	printMatrix(c,2,1,"c");
	c[0][0] = 0.;
	c[1][0] = 0.;
	printMatrix(c,2,1,"c");
	cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,2,1,4,1.0,a,4,b,1,0.0,c,1);
	printf("After sgemm\n");
	for(i = 0;i<2;i++){
		printf("c[%d]:",i);
		for(k=0;k<1;k++){
			printf("%f,",c[i][k]);
		}
		printf("\n");
	}

//	cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasTrans,/*m*/CO,/*n*/tmw,/*k*/tmh,/*alpha*/1.0,/*A*/&(conv_filter[0][0][0][0]),/*lda*/tmh,/*B*/&((*tm)[0][0]),/*ldb*/tmh,/*beta*/0.,/*C*/&(conv[c0][0][0][0]),/*ldc*/tmw);
}
