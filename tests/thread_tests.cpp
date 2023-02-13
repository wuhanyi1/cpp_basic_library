#include "common.h"
#include <iostream>
#include <thread>
#include <chrono>

void test_fun() {
    while (true)
    {
        std::cout << "test_fun" << std::endl;
        std::cout << "test_thread1's pid is:" << why::ThisThread::GetID() << std::endl;
        std::cout << "test_thread1's name is:" << why::ThisThread::GetName() << std::endl;
        why::ThisThread::SetName("why_test_name");
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

int main() {
    why::Thread t(test_fun, "test_thread1");
    std::cout << "main test_thread1's pid is:" << t.GetPid() << std::endl;
    std::cout << "main test_thread1's name is:" << t.GetName() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "main test_thread1's name is:" << t.GetName() << std::endl;
    t.Join();
}