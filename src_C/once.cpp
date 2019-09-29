#include <iostream>
#include <thread>
#include <mutex>
 
std::once_flag flag1;
 
void simple_do_once()
{
    std::thread::id this_id = std::this_thread::get_id();
    std::cout <<"Thread:"<<this_id<< "Simple example: called once\n"; 
}
 
 
void do_once(bool do_flag)
{
    std::thread::id this_id = std::this_thread::get_id();
    std::cout<<"Thread:"<<this_id<<" invoked!\n";
    std::call_once(flag1, simple_do_once);
}
 
int main()
{
    std::thread t1(do_once, true);
    std::thread t2(do_once, true);
    std::thread t3(do_once, false);
    std::thread t4(do_once, true);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    
}
