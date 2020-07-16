#include <iostream>
#include "TutorialConfig.h"

#ifdef USE_MYMATH
#include "MathFunctions.h"
#else
#include <cmath>
#endif

using namespace std;

int main(int argc , char * argv[]){
	if (argc < 2) {
	// report version
		cout << argv[0] << " Version " << Tutorial_VERSION_MAJOR << "."
		          << Tutorial_VERSION_MINOR << endl;
		cout << "Usage: " << argv[0] << " number" << endl;
		return (1);
	}
	double inputValue = stod(argv[1]);
#ifdef USE_MYMATH
  cout <<"USE_MYMATH"<<endl;
  const double outputValue = mysqrt(inputValue);
#else
  cout <<"NO USE_MYMATH"<<endl;
  const double outputValue = sqrt(inputValue);
#endif
	cout <<"arg0:"<< argv[0] << ",sqrt of "<< argv[1]<<" is "<<outputValue<<endl;
	return (0);
}
