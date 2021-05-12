#include <stdio.h>
int main(){
	unsigned int s = 8,st=-1;
	printf("s=%d,st=%d,st=%d\n",s,st,st);
	printf("\nsizeof(short)=%lu\nsizeof(int)=%lu\nsizeof(long)=%lu\nsizeof(unsigned long)=%lu\nsizeof(long long)=%lu\n",
		sizeof(short),
		sizeof(int),
		sizeof(long),
		sizeof(unsigned long),
		sizeof(long long)
		);
	return (0);
}
