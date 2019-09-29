#include <iostream>
const double PI  =3.141592653589793238463;

class Shape{
public:
	float totallen ;
	virtual void draw()=0;
    float getlen(){
		return(totallen);
	}
	void setlen(float x){
		totallen = x;
	}
	Shape(float l){
		totallen = l;
	}
};
class Rectangle: public Shape{
public:
	void draw(){
		printf("Rectangle.draw(),len=%f\n",getlen());
	}
	Rectangle(float l): Shape(l)
	{
		
	}
};
class Circle: public Shape{
public:
	float r;
	void draw(){
		printf("Circle.draw(),len = %f\n",getlen());
	}
	Circle (float x):Shape(0){
		
		r = x;
		setlen(2*r*PI);
	}
};

int main(){
	Circle cl(2.8);
	cl.draw();
	Rectangle rt(9.2);
	rt.draw();
	printf("main end.\n");
}
