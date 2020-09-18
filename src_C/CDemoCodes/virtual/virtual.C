#include <iostream>
using namespace std;

class Shape                    /* Abstract class */
{
	protected:
		float l;
	public:
		void get_data()          /* Note: this function is not virtual. */
		{
			cin>>l;
		}
		virtual float circle(){ return(0);}
		
		virtual float area() =0; /* Pure virtual function */
};

class Square : public Shape
{
	public:
		float area()
		{   return l*l;  }
		float circle(){return 4*l;}
};

class Circle : public Shape
{
	public:
		virtual float area()
		{ return 3.14*l*l; }
};

int main()
{
	Square s;
	Circle c;
	cout<<"Enter length to calculate area of a square: ";
	s.get_data();
	cout<<"Area of square: "<<s.area()<<",circle="<<s.circle()<<endl;
	cout<<"\nEnter radius to calcuate area of a circle:";
	c.get_data();
	cout<<"Area of circle: "<<c.area()<<",circle="<<c.circle()<<endl;
	return 0;
}
