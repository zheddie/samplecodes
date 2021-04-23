#include <stdio.h>
extern void func1();
extern void func2();
int main(int argc, char * argv[]) {
    printf("This in main.\n");
    func1();
    func2();
    return(0);

}