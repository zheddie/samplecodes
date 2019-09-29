/*
 * FreeArray.C
 *
 *  Created on: Jun 14, 2017
 *      Author: zhanggan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct people{
	int age;
	char name[20];
}People;
int main(){
	People * ps = (People *)malloc(10*sizeof(People));
	void * noise = NULL;
	for(int i =0 ; i< 10;i++){
		ps[i].age = i*2;
		sprintf(ps[i].name,"dada:%d",i);
	}
	for(int i =0 ; i< 10;i++){
			printf("ps[%d].age=%d,ps[%d].name=%s\n",i,ps[i].age ,i,ps[i].name);
	}
	People * pp = &ps[0];

	People * pt = &ps[1];

	free(pt);	//Free the first one only?
//	noise =  realloc(pt,200);
//	memset(noise,0,200);
	printf("ps=%p,pp=%p,pt=%p,noise=%p\n",ps,pp,pt,noise);
	printf("pt[0].age=%d,pt[0].name=%s\n",pt[0].age ,pt[0].name);

}
