#include <iostream>
using namespace std;
class szcl{
    int e;
public:
    szcl(){e=0;};
    szcl(int v){e = v;};
    int get_value(){return(e);};
};
int main (int argc , char * argv[]){
    szcl a1[3] = {3,5,7},*elem;
    for(int i=0;i<3;i++){
        cout<<a1[i].get_value()<<endl;
    }
    elem = a1;
    for(int i=0;i<3;i++){
        cout<<elem->get_value()<<endl;
        elem++;
    }
}