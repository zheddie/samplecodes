#include <stdio.h>
int main(){
	int zhishu[10500];
	zhishu[0] = 2;
	zhishu[1] = 3;
	zhishu[2] = 5;
	zhishu[3] = 7;
	zhishu[4] = 11;
	zhishu[5] = 13;
	int idx = 6, halfx=0,maxidx=0;
	int x=14;
	while(x<105000){
		halfx = x/2+1;
		for(int i =0;i<idx;i++){
			if(zhishu[i] >halfx){
				maxidx = i-1;
			}
		}
		for(int j=0;j<=maxidx;j++){
			if( x%zhishu[j] == 0){
				goto skip;
			}
		}
		zhishu[idx] = x;
		idx = idx+1;
skip:
		x=x+1;	
	}
	printf("ZhaoDaoZhiShuShuLiang:%d\n",idx);
	for(int k=0;k<idx;k++){
		if(k%10==0 && k!=0){
			printf("%d \n",zhishu[k]);
		}
		else{
			printf("%d ",zhishu[k]);
		}
	} 
}