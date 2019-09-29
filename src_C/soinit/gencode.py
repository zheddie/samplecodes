#!/usr/bin/env python3
#Try to generate code in sequence like below:
#void print0001(string x){ cout<<x<<"\n";}
import sys
mul = int(sys.argv[1])

print('''
#include <iostream>
#include "zgclasses.hpp"
namespace ZG{namespace sotest{
    namespace {
using namespace std;
'''
    )
for i in range(20000):
    print("void printA{}(string x){{ for (int i ={};i<{};i++){{cout<<x<<i<<endl;}}}}".format(i,i*(mul-1),i*mul))
print('''
zgRegisterOperators zgreg3({
''')
print("zgOperator(\"zg.schema.func{}.zgreg3.01\",\"hello Lamda in C++ zgreg3.01\"),".format(mul+2))
print("zgOperator(\"zg.schema.func{}.zgreg3.02\",\"hello Lamda in C++ zgreg3.02\"),".format(mul+2))
print('''
});}}}
    '''
)

