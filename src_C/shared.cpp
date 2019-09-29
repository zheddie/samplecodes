#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <string.h>
#include <memory.h> 
struct Base
{
    Base() { 
    	std::cout << "  Base::Base()\n"; 
	init("Cls:Base");
	}
    // Note: non-virtual destructor is OK here
    ~Base() { std::cout << "  Base::~Base()\n"; }
    void init(const char * s){
	strcpy(eyecatcher,s);
    }
private:
	char  eyecatcher[16]; 
};
 
struct Derived1: public Base
{
    Derived1(){ 
        int x = 0;
    	std::cout << "  Derived::Derived()\n"; 
	init("Cls:Derived");
	}
    ~Derived1() { std::cout << "  Derived::~Derived()\n"; }
private:
    const char p[16] = "CLS:BASE";
};
 
void thr(std::shared_ptr<Base> p)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout<<"in thr:p.get.cp0:"<< p.get()<<"\n";
    std::shared_ptr<Base> lp = p; // thread-safe, even though the
                                  // shared use_count is incremented
    std::cout<<"in thr:p.get.cp1:"<< p.get()<<"\n";
    {
        static std::mutex io_mutex;
        std::lock_guard<std::mutex> lk(io_mutex);
        std::cout << "local pointer in a thread:\n"
                  << "  lp.get() = " << lp.get()
                  << ", lp.use_count() = " << lp.use_count() << '\n';
    }
}
 
int main()
{
    std::shared_ptr<Base> p = std::make_shared<Derived1>();
 
    std::cout << "Created a shared Derived (as a pointer to Base)\n"
              << "  p.get() = " << p.get()
              << ", p.use_count() = " << p.use_count() << '\n';
    std::thread t1(thr, p), t2(thr, p), t3(thr, p);
    p.reset(); // release ownership from main
    std::cout << "Shared ownership between 3 threads and released\n"
              << "ownership from main:\n"
              << "  p.get() = " << p.get()
              << ", p.use_count() = " << p.use_count() << '\n';
    t1.join(); t2.join(); t3.join();
    std::cout << "All threads completed, the last one deleted Derived\n";
}
