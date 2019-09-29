#include <iostream>
#include <thread>
#include <mutex>
 
std::once_flag flag1;
std::mutex screen_lock;
 
void simple_do_once()
{
    std::thread::id this_id = std::this_thread::get_id();
    screen_lock.lock();
    std::cout <<"Thread:"<<this_id<< "Simple example: called once\n"; 
    screen_lock.unlock();
}
 
 
extern "C" void do_once(bool do_flag)
{
    std::thread::id this_id = std::this_thread::get_id();
    screen_lock.lock();
    std::cout<<"Thread:"<<this_id<<" invoked!\n";
    screen_lock.unlock();
    std::call_once(flag1, simple_do_once);
}
 
