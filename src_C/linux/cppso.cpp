#include <iostream>
extern "C" int print ( char * p)
{
	std::cout<<"In so. Hello:"<<p<<std::endl;
	return(0);
}
 
