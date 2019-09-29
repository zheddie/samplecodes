#include <stdio.h>
/*#define _XOPEN_SOURCE 700*/
#include <standards.h>
#include <pthread.h>

#define XSTR(x) STR(x)
#define STR(x) #x

#pragma message "The value of ZG: " XSTR(_ZG_)
int main(){



printf("zg.hello.pthread\n");
}
