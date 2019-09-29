#include <iostream>
using namespace std;
extern int pt;
void ttt(){
	static int tt=1;
	tt++;
	cout << "tt="<< tt<<endl;
	cout << "pt="<<pt<<endl;
}
	
