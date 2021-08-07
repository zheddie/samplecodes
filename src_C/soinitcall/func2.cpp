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


			extern "C" int callfunc(){
				zgOp *s = new zgOp(99);
				cout<<"["<<getpid()<<"]In func2.cpp.callfunc().cp0,idx="<<s->getidx()<<endl;
				return(0);
			}
		}
	}
}
