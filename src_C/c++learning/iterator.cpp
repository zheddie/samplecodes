#include <iostream>
#include <istream>
#include <map>
#include <list>
#include <iterator>
#include <algorithm>

using namespace std;
map<string,int> histogram;
void record(const string & s){
    histogram[s]++;
}
void print(const pair<const string,int> & r){
    cout<<r.first<<' '<<r.second<<endl;
}
int main(){
    istream_iterator<string> ii(cin);
    istream_iterator<string> eos;
    for_each(ii,eos,record);
    for_each(histogram.begin(),histogram.end(),print);
}