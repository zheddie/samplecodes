#include <stdio.h>
#include <stdlib.h>
#include <altivec.h>
#define VERBOSE 1
void
print (const char *name, const double *matrix, int row, int column)
{
  printf ("Matrix %s has %d rows and %d columns:\n", name, row, column);
  for (int i = 0; i < row; i++)
    {
      for (int j = 0; j < column; j++)
	{
	  printf ("%f ", matrix[i * column + j]);
	}
      printf ("\n");
    }
  printf ("\n");
}
typedef unsigned char vec_t __attribute__ ((vector_size (16)));
// typedef double v4sf_t __attribute__ ((vector_size (16)));
typedef double v2sf_t __attribute__ ((vector_size (8)));

#define SAVE_ACC(ACC, ldc,J)  \
	__builtin_mma_disassemble_acc (result, ACC); \
	prowC = (v2sf_t *) &CO[0*ldc+J]; \
	prowC[0] = result[0];\
	prowC[1] = result[2];\
	prowC[2] = result[4];\
	prowC[3] = result[6];\
	prowC = (v2sf_t *) &CO[1*ldc+J]; \
	prowC[0] = result[1];\
	prowC[1] = result[3];\
	prowC[2] = result[5];\
	prowC[3] = result[7];
// #define SAVE_ACC(ACC, ldc,J)  \
// 	__builtin_mma_disassemble_acc (result, ACC); \
// 	prowC = (v2sf_t *) &CO[0*ldc+J]; \
// 	prowC[0] = ((v2sf_t *)&result[0])[0];\
// 	prowC[1] = ((v2sf_t *)&result[1])[0];\
// 	prowC[2] = ((v2sf_t *)&result[2])[0];\
// 	prowC[3] = ((v2sf_t *)&result[3])[0];\
// 	prowC = (v2sf_t *) &CO[1*ldc+J]; \
// 	prowC[0] = ((v2sf_t *)&result[0])[1];\
// 	prowC[1] = ((v2sf_t *)&result[1])[1];\
// 	prowC[2] = ((v2sf_t *)&result[2])[1];\
// 	prowC[3] = ((v2sf_t *)&result[3])[1];\
// 	print("CO",CO,16,4);
void
MMA (int m, int n, int k, double *A, double *B, double *C)
{
  __vector_quad acc0, acc1, acc2, acc3, acc4, acc5, acc6, acc7;
  v2sf_t result[8];
  //v4sf_t *rowC;
  v2sf_t *prowC ;
  for (int l = 0; l < n; l += 4)
    {
      double *CO;
      double *AO;
      AO = A;
      CO = C;
      C += m * 4;
      for (int j = 0; j < m; j += 16)
	{
	  double *BO = B;
	  __builtin_mma_xxsetaccz (&acc0);
	  __builtin_mma_xxsetaccz (&acc1);
	  __builtin_mma_xxsetaccz (&acc2);
	  __builtin_mma_xxsetaccz (&acc3);
	  __builtin_mma_xxsetaccz (&acc4);
	  __builtin_mma_xxsetaccz (&acc5);
	  __builtin_mma_xxsetaccz (&acc6);
	  __builtin_mma_xxsetaccz (&acc7);
	  unsigned long i;

	  for (i = 0; i < k; i++)
	    {			
		vec_t *rowA = (vec_t *) & AO[i * 16];
		__vector_pair rowB;
		vec_t *rb = (vec_t *) & BO[i * 4];
	    //   __builtin_mma_assemble_pair (&rowB, rb[1], rb[0]);
		__builtin_mma_assemble_pair (&rowB, rb[0], rb[1]);
	      __builtin_mma_xvf64gerpp (&acc1, rowB, rowA[1]);
	      __builtin_mma_xvf64gerpp (&acc0, rowB, rowA[0]);
	      __builtin_mma_xvf64gerpp (&acc2, rowB, rowA[2]);
	      __builtin_mma_xvf64gerpp (&acc3, rowB, rowA[3]);
	      __builtin_mma_xvf64gerpp (&acc4, rowB, rowA[4]);
	      __builtin_mma_xvf64gerpp (&acc5, rowB, rowA[5]);
	      __builtin_mma_xvf64gerpp (&acc6, rowB, rowA[6]);
	      __builtin_mma_xvf64gerpp (&acc7, rowB, rowA[7]);
	    }
	 printf("zg.MMA().cp0,n=%d\n",n);
	  SAVE_ACC (&acc0, n,0);
	  SAVE_ACC (&acc1, n,2*n);
	  SAVE_ACC (&acc2, n,4*n);
	  SAVE_ACC (&acc3, n,6*n);
	  SAVE_ACC (&acc4, n,8*n);
	  SAVE_ACC (&acc5, n,10*n);
	  SAVE_ACC (&acc6, n,12*n);
	  SAVE_ACC (&acc7, n,14*n);
	  AO += k * 16;
	  BO += k * 4;
	  CO += 16;
	}
      B += k * 4;
    }
}

void
init (double *matrix, int row, int column)
{
	printf("zg.init().cp0,row=%d,column=%d\n",row,column);
  for (int j = 0; j < row; j++)
    {
      for (int i = 0; i < column; i++)
	{
		// matrix[j * row + i] = (i * 16 + 2 + j) / 0.123;
		// printf("zg.init(),cp01,j*row+i=%d,j:%d,i:%d,val=%f\n",j*row+1,j,i,(i+1)*(j+1)*1.0);
		matrix[j * column + i] = (i+1)*(j+1)*1.0;
	}
    }
	printf("zg.init().cp9,row=%d,column=%d\n",row,column);
}

void
init0 (double *matrix, double *matrix1, int row, int column)
{
  for (int j = 0; j < row; j++)
    for (int i = 0; i < column; i++)
      matrix[j * column + i] = matrix1[j * column + i] = 0;
}




int
main (int argc, char *argv[])
{
  int rowsA, colsB, common;
  int i, j, k;
  int ret = 0;


	  rowsA = 16;
	  colsB = 4;
	  common = 1;
	  if(argc > 3){
		  rowsA = atoi(argv[1]);
			colsB = atoi(argv[2]);
			common = atoi(argv[3]);
	  }
	  printf("zg.cp0,rowsA=%d,colsB=%d,common=%d\n",rowsA,colsB,common);
	  /* printf ("Running test for rows = %d,cols = %d\n", t, t1); */
	  double A[rowsA * common];
	  double B[common * colsB];
	  double C[rowsA * colsB];
	  double D[rowsA * colsB];
	  double AT[common * rowsA];

		printf("zg.cp1,rowsA=%d,colsB=%d,common=%d\n",rowsA,colsB,common);
	  init (A, rowsA, common);
	  printf("zg.cp2,rowsA=%d,colsB=%d,common=%d\n",rowsA,colsB,common);
	  print ("A", A, rowsA, common);
	  
	  
	  for (int i = 0; i < rowsA; i++)
        for (int j = 0; j < common; j++)
		{
			printf("AT:%d=A:%d=%f\n",j*rowsA+i,i*common+j, A[i*common+j]);
            AT[j*rowsA+i] = A[i*common+j];
		 } 
		 
		  print ("AT", AT, common,rowsA);
		  init (B, common, colsB);
	  print ("B", B, common, colsB);
	      init0 (C, D, rowsA, colsB);
	      print ("C", C, rowsA, colsB);
	      print ("D", D, rowsA, colsB);
	  MMA (rowsA, colsB, common, AT, B, C);
	  printf("zg.cp3,rowsA=%d,colsB=%d,common=%d\n",rowsA,colsB,common);
	  
//print ("C", C, rowsA, colsB);
	  for (i = 0;i<rowsA;i++){
		  for (j = 0;j<colsB;j++){
			  for(k = 0;k<common;k++){
				//   printf("zg.cp31,i:%d,j:%d,k:%d\n",i,j,k);
				 D[i*colsB+j] += A[i*common+k]*B[k*colsB+j];
			  }
		  }
	  }
	printf("zg.cp4,rowsA=%d,colsB=%d,common=%d\n",rowsA,colsB,common);
	  
	      print ("C", C, rowsA, colsB);
	      print ("D", D, rowsA, colsB);
	    // }

  
#ifdef VERBOSE
  if (ret)
    printf ("MMA double test fail: %d errors\n",ret);
  else
    printf ("MMA double test success: 0 MMA errors\n");
#else
  if (ret)
    abort();
#endif
      printf("zg.cp9,rowsA=%d,colsB=%d,common=%d\n",rowsA,colsB,common);
	  
  return ret;
}
