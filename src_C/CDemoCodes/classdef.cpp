#include <iostream>
using namespace std;
class CDate{
	public:
	CDate(); 
	void setDate(int y, int m, int d);
	void printIt();
	CDate *nextDate;
	private:
	int year,month,day;
	};
CDate::CDate(){
	year=month=day=0;
}
void CDate::setDate(int y,int m, int d){
	year = y;
	month = m;
	day = d;
	}
void CDate::printIt(){
	cout << "Date = " <<year<<"/"<<month<<"/"<<day<<endl;
	}
int main(){
	CDate aDate;
	aDate.nextDate = new CDate();
	aDate.setDate(2009,5,20);
	aDate.printIt();
	aDate.nextDate->printIt();
	return(0);
}
