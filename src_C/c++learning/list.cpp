#include <iostream>
void print(std::initializer_list<int> vals){
    for(auto p=vals.begin();p!=vals.end();p++){
        std::cout<<*p<<"\n";
    }
}
int main(){
    print({23,32,31,8,6,9});
}