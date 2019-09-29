#include <iostream>
namespace zg{
using namespace std;
struct OperatorSet{
	OperatorSet(initializer_list<const char *> sig_literals);
private:
	unordered_map<Symbol,vector<shared_ptr<Operator>>> ops;
}

