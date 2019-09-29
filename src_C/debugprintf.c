#include <stdio.h>
#define DEBUGPRINTF(f_, ...) printf((f_), __VA_ARGS__)
int main(){
	DEBUGPRINTF("zg.debug.f=%d\n",2);
	return(0);
}
