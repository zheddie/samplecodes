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
bool gt_4(const pair<const string,int> &r){
    return r.second>4;
}
int main(){
    istream_iterator<string> ii(cin);
    istream_iterator<string> eos;
    for_each(ii,eos,record);
    for_each(histogram.begin(),histogram.end(),print);
    // typedef map<string,int>::const_iterator MI;
    using  MII = std::map<std::__cxx11::basic_string<char>, int>::iterator;
    MII i  = find_if(histogram.begin(),histogram.end(),gt_4);
    // i  = find_if(i,histogram.end(),gt_4);
    while(i!= histogram.end()){
        print(*i);
        ++i;
        i  = find_if(i,histogram.end(),gt_4);
    }
    int ct = count_if(histogram.begin(),histogram.end(),gt_4);
    cout<<"count:"<<ct<<endl;
}