#include <iostream>
#include <thread>
#include <mutex>
#include <dlfcn.h>
#include <errno.h>
void (* pdo_once)(bool do_flag);

 
int fake_main(){

    void * handle = dlopen("_C.so",RTLD_LOCAL|RTLD_NOW);
    if(handle == NULL){
	    std::cout<<"dlopen failed.errno:"<<errno<<std::endl;
	    return(-1);
    }
    pdo_once = (void (*)(bool))dlsym(handle,"do_once");
    if(pdo_once == NULL){
    	std::cout << "do_once failed to dlsym.errno:"<<errno<<std::endl;
	return(-1);
    }
    std::thread t1(*pdo_once, true);
    std::thread t2(*pdo_once, true);
    std::thread t3(*pdo_once, false);
    std::thread t4(*pdo_once, true);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    dlclose(handle);
    return(0);
} 
