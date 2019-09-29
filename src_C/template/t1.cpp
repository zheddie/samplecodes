#include <iostream>

template<typename T>void foo(T f)
{
    std::cout << f << std::endl;
}

int main()
{
    foo(3.5);

    return 0;
}
