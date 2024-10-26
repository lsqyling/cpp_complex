//
// Created by 10580.
//

#include <thread>
#include <iostream>

void hello()
{
    std::cout << "hello" << std::endl;
}






int main()
{
    std::thread t(hello);
    t.join();


    return 0;
}