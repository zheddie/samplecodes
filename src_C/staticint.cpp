#include <iostream>
using namespace std;
int pt;
void ppp();
void ttt();
int main(){
	ppp();
	cout <<"pt="<<pt<<endl;
	ppp();
	ttt();
	return(0);
}
void ppp(){
	static int si=1;
	si++;
	cout <<"si="<<si<<endl;
	cout <<"pt="<<pt<<endl;
}
