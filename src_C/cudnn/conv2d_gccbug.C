#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef VERIFY
#define VERIFY 0
#endif

int randomnumber(int b)
{
    return rand()%(b+1);
}
void d4_print(const char * name,float **** mat,int c0,int c1,int c2,int c3){
	printf("%s:\n",name);
	int i=0,j=0,k=0,l=0;
	for(i=0;i<c0;i++){
		printf("{");
		for(j=0;j<c1;j++){
			printf("(");
			for(k=0;k<c2;k++){
				printf("[");
				for(l=0;l<c3;l++){
					printf("%.2f",mat[i][j][k][l]);
					if (l<c3-1) printf(",");
				}
				printf("]");
			}
			printf(")\n");
		}
		printf("}\n");
	}	
}
void d2_print(const char * name,float ** mat,int c0,int c1){
	printf("%s:\n",name);
	int i=0,j=0,k=0,l=0;
	for(i=0;i<c0;i++){
		printf("[");
		for(j=0;j<c1;j++){
			printf("%.2f",mat[i][j]);
			if (j<c1-1) printf(",");
		}
		printf("]\n");
	}	
}
float **** d4_randconstruct(int c0,int c1,int c2,int c3){
	int i=0,j=0,k=0,l=0;
	float **** mat = NULL;
	mat = (float ****)malloc(sizeof(float ***)*c0);
	for(i=0;i<c0;i++){
		mat[i] = (float ***)malloc(sizeof(float **)*c1);
		for(j=0;j<c1;j++){
			mat[i][j] = (float **)malloc(sizeof(float *)*c2);
			for(k=0;k<c2;k++){
				mat[i][j][k] = (float *)malloc(sizeof(float) * c3);
				for(l=0;l<c3;l++){
					mat[i][j][k][l] = randomnumber(255);
				}
			}
		}
	}
	return(mat);
}
float **** d4_zeroconstruct(int c0,int c1,int c2,int c3){
	int i=0,j=0,k=0,l=0;
	float **** mat = NULL;
	mat = (float ****)malloc(sizeof(float ***)*c0);
	for(i=0;i<c0;i++){
		mat[i] = (float ***)malloc(sizeof(float **)*c1);
		for(j=0;j<c1;j++){
			mat[i][j] = (float **)malloc(sizeof(float *)*c2);
			for(k=0;k<c2;k++){
				mat[i][j][k] = (float *)malloc(sizeof(float) * c3);
				memset(mat[i][j][k],0,sizeof(float )* c3);
			}
		}
	}
	return(mat);
}

float ** d2_randconstruct(int c0,int c1){
	int i=0,j=0;
	float ** mat = NULL;
	mat = (float **)malloc(sizeof(float *)*c0);
	for(i=0;i<c0;i++){
		mat[i] = (float *)malloc(sizeof(float )*c1);
		for(j=0;j<c1;j++){
			mat[i][j] = randomnumber(255);
		}
	}
	return(mat);
}

float ** d2_zeroconstruct(int c0,int c1){
	int i=0,j=0;
	float ** mat = NULL;
	mat = (float **)malloc(sizeof(float *)*c0);
	for(i=0;i<c0;i++){
		mat[i] = (float *)malloc(sizeof(float )*c1);
		memset(mat[i],0,sizeof(float )* c1);
	}
	return(mat);
}

int div_up(int a, int b){
	if(a%b == 0 ){
		return( a/b);
	}else{
		return(a/b+1);
	}
}
float ** d2_mulmat(float ** out,float ** in0,float ** in1,int in0_c0,int in0_c1,int in1_c1){
	int i=0,j=0,k=0;

	for(i = 0;i<in0_c0;i++){
		for(j=0;j<in1_c1;j++){
			float tmp=0.;
			for(k=0;k<in0_c1;k++){
				tmp+=in0[i][k]*in1[k][j];
			}
			out[i][j] = tmp;
		}
	}
	return(out);
}
float **** conv2d(float **** out,float ****img,float ****flt,int img_c0,int img_c1,int img_c2,int img_c3,int flt_c0,int flt_c1,int flt_c2,int out_h,int out_w){
	int c0=0,c1=0,c2=0,c3=0;
	int s0=0,s1=0,s2=0,s3=0;
	int s1_max = 0, s2_max = 0;
/*	out = (float ****)malloc(sizeof(float ***)*img_c0);*/

	for(c0=0;c0<img_c0;c0++){
		/*go through each image*/
		/*now start the conv for each filter*/
		for(c3=0;c3<flt_c0;c3++){
			for(c1=0;c1<out_h;c1++){
				/*for each height*/
				for(c2=0;c2<out_w;c2++){
					/*for each point in the output matrix, we need do a conv*/
					s1_max = flt_c1;
					if(flt_c1 > img_c2-c1){/*others are zeros, do not need to do*/
						s1_max = img_c2 -c1;
					}
					for(s1=0;s1<s1_max;s1++){
						s2_max = flt_c2;
						if(flt_c2 > img_c3-c2){/*others are zeros, do not need to do*/
							s2_max = img_c3 -c2;
						}						
						for(s2=0;s2<s2_max;s2++){
							for(s3=0;s3<img_c1;s3++){
								out[c0][c3][c1][c2]+=img[c0][s3][c1+s1][c2+s2]*flt[c3][s1][s2][s3];
							}
						}
					}
				}
			}
		}
	}
	return(out);
}
/*conv2d_im2col(convout,images,filters,img_count,img_h,img_w,img_chl,flt_count,flt_h,flt_w,conv_h,conv_w,om,fm,om_h,om_w,fm_h,fm_w);*/

float **** conv2d_im2col(float **** out,float ****img,float ****flt,int img_c0,int img_c1,int img_c2,int img_c3,int flt_c0,int flt_c1,int flt_c2,int out_h,int out_w,float ** om,float ** fm,int om_h,int om_w,int fm_h,int fm_w){

	int c0=0,c1=0,c2=0,c3=0;
	int d0=0,d1=0,d2=0,d3=0;
	int s0=0,s1=0,s2=0,s3=0;
	int flt_c1_max = 0, flt_c2_max = 0;
	/*try to transform filter firstly*/
	int flt_count_per_feature = flt_c1*flt_c2;
	for(c0=0;c0<flt_c0;c0++){
		for(c1=0;c1<img_c1;c1++){ /*channels*/
			for(c2=0;c2<flt_c1;c2++){
				for(c3=0;c3<flt_c2;c3++){
					fm[c0][c1*flt_count_per_feature+c2*flt_c1+c3] = flt[c0][c2][c3][c1];
				}
			}
		}
	}
	if(VERIFY) d2_print("fm",fm,fm_h,fm_w);
	for(d0=0;d0<img_c0;d0++){
		/*go through each image*/
		/*try to transform each image*/
		for(s0=0;s0<om_w;s0++){
			for(d1=0;d1<img_c1;d1++){
				flt_c1_max = flt_c1;
				if(flt_c1 > img_c2-s0/out_w){/*others are zeros, do not need to do*/
					flt_c1_max = img_c2 -s0/out_w;
				}
				for(d2=0;d2<flt_c1_max;d2++){
					flt_c2_max = flt_c2;
					if(flt_c2 > img_c3-s0%out_w){/*others are zeros, do not need to do*/
						flt_c2_max = img_c3 -s0%out_w;
					}
					for(d3=0;d3<flt_c2_max;d3++){
						om[d1*flt_count_per_feature+d2*flt_c1+d3][s0] = img[d0][d1][s0/out_w+d2][s0%out_w+d3];
					}
				}
			}
		}
		/*do the mulmat now for one image*/
		if(VERIFY) d2_print("om",om,om_h,om_w);
		for(s1=0;s1<fm_h;s1++){
			for(s2=0;s2<om_w;s2++){
				float tmp =0.;
				for(s3 = 0;s3<fm_w;s3++){
					tmp+= fm[s1][s3]*om[s3][s2];
				}
				out[d0][s1][s2/out_w][s2%out_w]=tmp;
			}
		}
	}

	return(out);
}

/*
img[n,3,7,7]
flt[16,3,3,3] [new_chl,h,w,chl]
out[n,16,7,7]
conv2d:
40*16*7*7*3*3*3(chl)*(2+1+1)(2add,1mul,1assign)=3386880
conv2d_im2col:
16*40*3*3+40*(49*3*3*(2+2+4+1)(/,+,%,+,*,+,*,+,=)+16*49*9*3*(3)(*,+,=)+16*49*(3)(/,%,=)) = 2798760
Increased in theory: (3386880-2798760)/3386880=17.364%

*/
int main(){
	float  **** images = NULL;
	float **** filter = NULL;
	float **** convout = NULL;
	float ** fm = NULL;
	float ** om = NULL;
	int conv_h = 0,conv_w=0;
	clock_t clocktime = 0;
	unsigned long long timeconsumed1 = 0,timeconsumed2 = 0;
	int i=0,j=0,k=0;
	int img_count,img_chl,img_h,img_w,img_padding;
	int flt_count,flt_h,flt_w,flt_stride ;
	if(VERIFY){
		img_count=1;
		img_chl=2;
		img_h=3; 
		img_w=3;
		img_padding=1;
		flt_count=2;
		flt_h=2;
		flt_w=2;
		flt_stride=1;
	}
	else {
		img_count=40;
		img_chl=3;
		img_h=224; 
		img_w=224;
		img_padding=2;
		flt_count=16;
		flt_h=3;
		flt_w=3;
		flt_stride=1;
	}



/* verify data: */


	if(VERIFY){


/*Try to verify the method*/
		images = d4_randconstruct(img_count,img_chl,img_h,img_w);
		for(i=0;i<img_w;i++){
			images[0][0][0][i] = 1;
			images[0][0][1][i] = 3;
			images[0][0][2][i] = 5;
			images[0][1][0][i] = 2;
			images[0][1][1][i] = 4;
			images[0][1][2][i] = 6;
		}
		d4_print("images",images,img_count,img_chl,img_h,img_w);
		filter = d4_randconstruct(flt_count,flt_h,flt_w,img_chl);
		for(i=0;i<flt_w;i++){
			filter[0][0][i][0] = 2;
			filter[0][1][i][0] = 4;
			filter[0][0][i][1] = 3;
			filter[0][1][i][1] = 5;

			filter[1][0][i][0] = 6;
			filter[1][1][i][0] = 8;
			filter[1][0][i][1] = 7;
			filter[1][1][i][1] = 9;
		}
		d4_print("filter",filter,flt_count,flt_h,flt_w,img_chl);
		conv_h = div_up((img_h+img_padding-flt_h+1),flt_stride);
		conv_w = div_up((img_w+img_padding-flt_w+1),flt_stride);
		convout = d4_zeroconstruct(img_count,flt_count,conv_h,conv_w);
		conv2d(convout,images,filter,img_count,img_chl,img_h,img_w,flt_count,flt_h,flt_w,conv_h,conv_w);
		printf("conv2d:\n");
		d4_print("convout",convout,img_count,flt_count,conv_h,conv_w);
		int fm_h = flt_count;
		int fm_w = flt_h*flt_w*img_chl;
		int om_h = fm_w;
		/*om_w = img_count * conv_h*conv_w;*/
		/*prefer to reuse one talbe for each image.*/	
		int om_w = conv_h*conv_w;
		fm = d2_zeroconstruct(fm_h,fm_w);
		om = d2_zeroconstruct(om_h,om_w);
		conv2d_im2col(convout,images,filter,img_count,img_chl,img_h,img_w,flt_count,flt_h,flt_w,conv_h,conv_w,om,fm,om_h,om_w,fm_h,fm_w);
		printf("conv2d_im2col:\n");
		d4_print("convout",convout,img_count,flt_count,conv_h,conv_w);
/*Try to verify a mulmat*/
		float ** one = d2_randconstruct(1000,1000);
		float ** two = d2_randconstruct(1000,1000);
		float ** mul = d2_zeroconstruct(1000,1000);
		clocktime = clock();
		d2_mulmat(mul,one,two,1000,1000,1000);
		timeconsumed1 = clock()-clocktime;
		printf("Timeconsumed(mulmat,1000,1000,1000): %llu/%d\n",timeconsumed1,CLOCKS_PER_SEC);		
	}else{
/*Try randomdata*/
		srand((unsigned)time(NULL));

		images = d4_randconstruct(img_count,img_chl,img_h,img_w);
		/*d4_print(images,img_count,img_h,img_w,img_chl);*/
		srand((unsigned)time(NULL));
		filter = d4_randconstruct(flt_count,flt_h,flt_w,img_chl);
		/*d4_print(filter,flt_count,flt_h,flt_w,img_chl);*/
		conv_h = div_up((img_h+img_padding-flt_h+1),flt_stride);
		conv_w = div_up((img_w+img_padding-flt_w+1),flt_stride);
		convout = d4_zeroconstruct(img_count,flt_count,conv_h,conv_w);
		clocktime = clock();
		conv2d(convout,images,filter,img_count,img_chl,img_h,img_w,flt_count,flt_h,flt_w,conv_h,conv_w);
		timeconsumed1 = clock()-clocktime;
		printf("Timeconsumed(old): %llu/%d\n",timeconsumed1,CLOCKS_PER_SEC);
		int fm_h = flt_count;
		int fm_w = flt_h*flt_w*img_chl;
		int om_h = fm_w;

		/*om_w = img_count * conv_h*conv_w;*/
		/*prefer to reuse one talbe for each image.*/
		
		int om_w = conv_h*conv_w;
		fm = d2_zeroconstruct(fm_h,fm_w);
		om = d2_zeroconstruct(om_h,om_w);
		clocktime = clock();
		conv2d_im2col(convout,images,filter,img_count,img_chl,img_h,img_w,flt_count,flt_h,flt_w,conv_h,conv_w,om,fm,om_h,om_w,fm_h,fm_w);
		timeconsumed2 = clock()-clocktime;
		printf("Timeconsumed(new): %llu/%d\n",timeconsumed2,CLOCKS_PER_SEC);
		printf("Increase(%%):%f\n",(float)(timeconsumed1-timeconsumed2)/timeconsumed1*100);
	}

}

