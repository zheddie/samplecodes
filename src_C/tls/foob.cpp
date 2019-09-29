#include <iostream>
#include "foo.hpp"
extern thread_local foo foo_instance;
extern thread_local int sx;
int main(int argc, char** argv)
  {
  double bar = foo_instance.bar();
  std::cout<<"bar:"<<bar<<",sx="<<sx<<std::endl;
  return 0;
  }
