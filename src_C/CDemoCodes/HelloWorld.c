#include <stdio.h>
#include <strings.h>

#define AA(X) printf("AA Defined%d\n",X)
#ifdef AA
#define AA() printf("AA redefined\n")
#endif
int main() {
        AA();
	AA(8);
	printf("HelloWorld!\n");
}
