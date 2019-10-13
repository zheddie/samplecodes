#include <iostream>
#include <thread>
#include <mutex>
#include <dlfcn.h>
#include <errno.h>
void (* pdo_once)(bool do_flag);
int fake_main();
 
int main(){
	return fake_main();
} 
