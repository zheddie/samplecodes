#include <stdio.h>
//#define DEBUG 0
//
typedef struct s{
	int ages[20];
} S;

#define SIZE (sizeof(((S*)NULL)->ages))
#define LEN (sizeof(((S*)NULL)->ages)/sizeof(((S*)NULL)->ages[0]))
main(){
	printf("ages size=%d,ages len=%d\n",SIZE,LEN);
	printf("DEBUG defined\n");
	return(0);
	}



