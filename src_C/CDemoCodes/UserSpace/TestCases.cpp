//============================================================================
// Name        : TestCases.cpp
// Author      : zg
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "UserSpace.H"
#include "mrdbIfsCfg.H"
using namespace std;



int main() {
	//cout << "!!!zg.Hello World!!!" << endl; // prints !!!Hello World!!!
	MrdbiASPName_t iasp;
	MrdbSystemName_t sysName;
	memset(iasp,0,sizeof(MrdbiASPName_t));
	memset(sysName,0,sizeof(MrdbSystemName_t));

	strcpy(iasp,"IASP1");
	strcpy(sysName,"sysZG1");
	mrdbAddIfsiAsp(&iasp,&sysName);

	return 0;
}

