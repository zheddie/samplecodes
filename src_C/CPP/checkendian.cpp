#include <iostream>
using namespace std;
int is_big_endian(void) {
  union {
    uint32_t i;
    char c[4];
  } bint = {0x01020304};

  return bint.c[0] == 1;
}

int main()
{
	if(is_big_endian()){
		cout<<"BIG endian system"<<endl;
	}else{
		cout<<"LITTLE endian system"<<endl;
	}
}
