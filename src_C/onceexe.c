#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

int fake_main();


int main(){
	fake_main();
	return(0);
} 
