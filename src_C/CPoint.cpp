#include <iostream>
#include <CPoint.h>
using namespace std;
CPoint::CPoint(int x=0,int y=0){
	printf("zg.this=%p\n",this);
	printf("zg.&this=%p\n",&this);
	xP = x;
	yP = y;
}
CPoint::CPoint(const CPoint &y){
	xP = y.yP;
	yP = y.xP;
}
void CPoint::copyPoint(CPoint b){
	xP = b.xP;
	yP = b.yP;
}
void CPoint::setPoint(int x = 0,int y = 0){
	xP = x;
	yP = y;
}
void CPoint::printPoint(){
	cout <<"X="<<xP<<",Y="<<yP<<endl;
}
void CPoint::move(int xO = 0,int yO = 0){
	xP += xO;
	yP += yO;
}

