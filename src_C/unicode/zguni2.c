#include <stdio.h>
#include <wchar.h>
int main(){
	char * s = "ab\u0144cd";
	char * t = "abńcd";
	printf("zg.s=%s\n",s);
	printf("zg.t=%s\n",t);
}
