#include <iostream>
#include <vector>
int main(int argc, char * argv[]){
    std::vector<int> mv = {3,5,8,6,9};
    mv.push_back(25);
    mv.push_back(13);
    std::cout << "v = { ";
    for (int n : mv) {
        std::cout << n << ", ";
    }
    std::cout << "}; \n";    
}