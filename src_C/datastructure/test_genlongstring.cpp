#include <iostream>
#include <stdlib.h>
using namespace std;
int main(int argc,char *argv[]){
    srand((unsigned)time(0));
    int idx = 0;
    for(int i ;i<100*1000*1000;i++){
        idx = rand() % 2;
        char c = 'a'+(char) idx;
        cout<<c;
    }
    cout<<endl;
}