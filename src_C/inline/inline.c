#include <stdio.h>
inline void ss();
void ss(){
	printf("hello inline.\n");
}

int main(){

	ss();
	return(0);
}
