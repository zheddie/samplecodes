#include <stdio.h>
#include <cblas.h>
#include <stdarg.h>
#include <string.h>
// void printmatrix(int **x,...){
//     va_list args;
//     va_start(args,x);
//     int argno = 0;
//     while(1){
//         char *p;
//         p = va_arg(args, char *);
//         if(strcmp(p, "") == 0) break;
//         printf("Parameter #%d is : %s\n", argno, p);
//         argno++;
//     }
// }
void printmatrix(char * n,float *x,int r,int c){

    for(int i=0;i<r;i++){
        printf("%s[%d]=",n,i);
        for(int j=0;j<c;j++){
            printf("%f ",x[i*c+j]);
        }
        printf("\n");
    }
    printf("\n");
}
int domult(){
	//I wish to do a matrix multiply here. For example , a(3,4)*b(4,2) = c(3,2)
    float a[3][4] = {{2.,4.,1.,2.},{5.,8.,7.,2.},{1.,0.,8.,2.}};
    float b[4][2] = {{2.,1.},{3.,5.},{2.,2.},{5.,3.}};
    float c[3][2] ;
    memset(c,0,sizeof(float)*2*3);
    printmatrix("a",&a[0][0],3,4);
    printmatrix("b",&b[0][0],4,2);
    for(int i =0;i<3;i++){
        for(int j=0;j<2;j++){
            float sum = 0;
            for(int k=0;k<4;k++){
                sum+=a[i][k]*b[k][j];
            }
            c[i][j]=sum;
        }
    }
    printmatrix("c",&c[0][0],3,2);
    memset(c,0,sizeof(float)*2*3);
    cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,/*m*/3,/*n*/2,/*k*/4,/*alpha*/1.0,/*A*/&(a[0][0]),/*lda*/4,/*B*/&(b[0][0]),/*ldb*/2,/*beta*/0.,/*C*/&(c[0][0]),/*ldc*/2);      
    printmatrix("c",&c[0][0],3,2);
    return(0);
}
