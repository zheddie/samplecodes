#include <iostream>
using namespace std;
class CPoint{
	public:
	CPoint(int,int);
	CPoint(const CPoint & );
	void setPoint(int , int);
	void printPoint();
	void copyPoint(CPoint b);
	void move(int,int);
	private:
	int xP,yP;
};
