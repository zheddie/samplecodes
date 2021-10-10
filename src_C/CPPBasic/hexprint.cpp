#include <iostream>
#include <cstring>
using namespace std;
struct student{
    char name[20];
    int age;
    bool isboy;
};
int main(){
    struct student kx;
    struct student *pkx = &kx;
    int *p = &(pkx->age);
    memset(pkx,0,sizeof(struct student));
    strcpy(pkx->name ,"kaixin");
    *p = 12;
    pkx->isboy = false;
    return(0);
}