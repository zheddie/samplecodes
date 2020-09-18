#include <iostream>
#include <cmath>

using namespace std;
double mysqrt(double x){
#if defined(HAVE_LOG) && defined(HAVE_EXP)
  double result = exp(log(x) * 0.5);
  cout << "Computing sqrt of " << x << " to be " << result
            << " using log and exp" << endl;
#else
  double result = sqrt(x);
  cout<<"This is in mysqrt.using sqrt:x:"<<x<<".result:"<<result<<endl;
#endif
	return(result);
}