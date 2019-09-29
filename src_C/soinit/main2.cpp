#include <iostream>
#include <dlfcn.h>
#include <errno.h>
int (* pf)();

int main(){
	
    //void * handle = dlopen("libfunc1.so",RTLD_NOW);
    void * handle = dlopen("libfunc1.so",RTLD_LAZY|RTLD_GLOBAL);
    if(handle == NULL){
	    std::cout<<"dlopen failed.errno:"<<errno<<std::endl;
	    return(-1);
    }
 //    pf = (int (*)())dlsym(handle,"func1");
 //    if(pf == NULL){
 //    	std::cout << "func1 failed to dlsym.errno:"<<errno<<std::endl;
	// return(-1);
 //    }
 //    (*pf)();
 //     pf = (int (*)())dlsym(handle,"func2");
 //    if(pf == NULL){
 //        std::cout << "func2 failed to dlsym.errno:"<<errno<<std::endl;
 //    return(-1);
 //    }
 //    (*pf)();   
 //    pf = (int (*)())dlsym(handle,"getstaticint");
 //    if(pf == NULL){
 //    	std::cout << "getstaticint failed to dlsym.errno:"<<errno<<std::endl;
	// return(-1);
 //    }
 //    for(int i=8;i<12;i++){
 //    	std::cout<<i-7<<":"<<(*pf)()<<std::endl;
 //    }

    dlclose(handle);

    return(0);

}
