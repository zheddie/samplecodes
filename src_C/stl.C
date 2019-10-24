#include <iostream>
#include <vector>
#include <ctime>
using namespace std;;

int main ( int argc, char **argv) {

    time_t now1 = time(0);
    std::vector<int> vec;
    vec.push_back(6);
    vec.push_back(-17);
    vec.push_back(12);
    for ( int i = 0; i<10000; i++) {
      vec.push_back(i);
    }

    time_t now2 = time(0);
    vec.erase(vec.begin() + 1);

    time_t now3 = time(0);
    cout << (now2 - now1) << " " << (now3 - now2)  << endl;
}
