#include <iostream>
using namespace std;
class MyClass {
  int i;
  public:
  MyClass() { 
    i=0; 
  }
  MyClass(int j) { 
      i=j; 
 }
   int getInt() { 
        return i; 
  }
 };
 int main()
 {
    MyClass myObject[5] = {1, 2, 3,4,5};
    MyClass *objectPointer;
    int i;
    objectPointer = myObject; 
    for(i=0; i<5; i++) {
        cout << objectPointer->getInt() << "\n";
        objectPointer++; 
    }
    return 0;
}
