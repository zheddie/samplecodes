#include <iostream>
#include <dlfcn.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

namespace ZG{
	namespace sotest{
		namespace {
			using namespace std;

			class zgOp{
			public:
				zgOp() = default;
				zgOp(int x){
					idx = x;
				}
				int getidx(){return(idx);}
			private:
				int idx;
			};
			// extern "C" __attribute__((constructor))  int autoinit(){
			extern "C" int __attribute__((constructor))  autoinit(){
				// zgOp *s = new zgOp(9);
				pid_t ch = 0;
				cout<<"["<<getpid()<<"]In func1.cpp.autoinit().cp0."<<endl;
				sleep(2);
				void * handle = dlopen("libfunc1.so",RTLD_NOW);
				if(handle == NULL){
					cout<<"["<<getpid()<<"]dlopen failed.errno:"<<errno<<endl;
					return(-1);
				}
				cout<<"["<<getpid()<<"]In func1.cpp.autoinit().cp12,libfunc1.so loaded again.handle="<<static_cast<void*>(handle)<<endl;
				
				// if(ch = fork()){
				// 	cout<<"["<<getpid()<<"]In func1.cpp.autoinit().cp11.child PID:"<<ch<<endl;
				// 	void * handle = dlopen("libfunc1.so",RTLD_NOW);
				// 	if(handle == NULL){
				// 		cout<<"["<<getpid()<<"]dlopen failed.errno:"<<errno<<endl;
				// 		return(-1);
				// 	}
				// 	cout<<"["<<getpid()<<"]In func1.cpp.autoinit().cp12,libfunc1.so loaded again.handle="<<static_cast<void*>(handle)<<endl;
				// }else{
				// 	cout<<"["<<getpid()<<"]In func1.cpp.autoinit().cp21"<<endl;
				// 	void * handle = dlopen("libfunc1.so",RTLD_NOW);
				// 	if(handle == NULL){
				// 		cout<<"["<<getpid()<<"]dlopen failed.errno:"<<errno<<endl;
				// 		return(-1);
				// 	}
				// 	cout<<"["<<getpid()<<"]In func1.cpp.autoinit().cp22,libfunc1.so loaded again.handle="<<static_cast<void*>(handle)<<endl;
				// }
				cout<<"["<<getpid()<<"]In func1.cpp.autoinit().cp9"<<endl;
				return(0);

			}

			extern "C" int callfunc(){
				zgOp *s = new zgOp(88);
				cout<<"["<<getpid()<<"]In func1.cpp.callfunc().cp0,idx="<<s->getidx()<<endl;
				// void * handle = dlopen("libfunc1.so",RTLD_NOW);
				// if(handle == NULL){
				// 	cout<<"[PARENT]dlopen failed.errno:"<<errno<<endl;
				// 	return(-1);
				// }
				// cout<<"[PARENT]In func1.cpp.callfunc().cp9.handle="<<static_cast<void*>(handle)<<endl;
				return(0);

			}
		}
	}
}
