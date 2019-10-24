// #include <iostream>
#include <stdio.h>
#include <map>
//#include <cstdio>
using namespace std;
int main(){
	    std::FILE * fp = std::fopen("demo.cpp","r");
	    char buf[200];
	    int cnt = std::fread(buf,200,1,fp);
	    //int pos = std::fgetpos(fp,33,22);
        std::printf("hello readcontent:%d:\n%s",cnt,buf);
#if __cplusplus <= 201103L
        std::printf("#if __cplusplus <= 201103L\n");
#else
        std::printf("#if __cplusplus > 201103L\n");
#endif
        std::fclose(fp);
        return(0);
}
