#include <stdio.h>
int main(){
    union {
	long long srcLong;
        char bt[8];
    } init;
    init.srcLong = 9007199791611905; 
    printf("srcLong=%lld,bt=0X",init.srcLong);
    for(int i =7;i>=0;i--){
	printf("%02X",init.bt[i]);
    }
    printf("\n");
    //long int srcLong = 9007199791611905; 
    union {
      float dstFloat;
      int intbits;
      char bt[4];
    }test;
    test.dstFloat = (float)init.srcLong;
    printf("test.dstFloat is %12.8e, intbits is %d/%x,scrcLong=%lld/%llx.\n",test.dstFloat, test.intbits,test.intbits,init.srcLong,init.srcLong);;
}
