#include <iostream>
using namespace std;
class base 
{
 public:
   virtual void vfunc(void)
    {
      cout << "This is base's vfunc()." << endl;
    }
};

class derived1 : public base 
{
 public:
   void vfunc(void)
    {
      cout << "This is derived1's vfunc()." << endl;
    }
};

class derived2 : public derived1 
{
 public:
   void vfunc(void)
   {
      cout << "This is derived2's vfunc()." << endl;
   }
};

int main(void)
{
   base *p, b;
   derived1 d1;
   derived2 d2;

   p = &b;        // Point to base class
   p->vfunc();
   p = &d1;       // Point to first derived class
   p->vfunc();
   p = &d2;       // Point to second derived class
   p->vfunc();
}
