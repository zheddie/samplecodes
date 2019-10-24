#include <iostream>
using namespace std;
class BaseClass {
  int i;
  public:
    void setInt(int num) { 
        i = num; 
	  }
	    int getInt() { 
	        return i; 
		  }
		  };
		  class derived: public BaseClass {
		    int j;
		    public:
		      void setJ(int num) { 
		           j = num; 
			     }
			       int getJ() { 
			            return j; 
				      }
				      };
int main()
{
BaseClass *baseClassPointer;
  derived d;
  baseClassPointer = &d;     // BaseClass pointer points to derived object
 // access derived object using BaseClass pointer
baseClassPointer->setInt(10);
return 0;
									       }
