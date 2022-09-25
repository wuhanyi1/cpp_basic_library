#include "log.h"
#include "common.h"
#include <thread>
#include <chrono>
#include <memory>
using namespace why;

void test_func() {
    LOG_INFO("test_func:%s", "1111");
}

void test_func2() {
    LOG_INFO("test_func2:%s", "1111");
}

int test_func3(int a) {
    return 11 + a;
}

int main() {
    // int n = 1e5;
    // auto p1 = std::chrono::steady_clock::now().time_since_epoch();
    // while (n--) {
    //     LOG_INFO("第一次测试：%s", "111111111111111111111111");
    // }
    // auto p2 = std::chrono::steady_clock::now().time_since_epoch();;
    // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(p2 - p1).count() << std::endl;

    // why::ThreadPool pool;
    // pool.Start();
    // pool.execute(test_func);
    // pool.execute(test_func2);
    // pool.execute([] {
    //     std::cout << "heihei" << std::endl;
    // });
    // int a = 10;
    // auto fut = pool.execute(test_func3, a);
    // std::cout << fut.get() << std::endl;
    // pool.Shutdown();
    // std::make_shared<Timer::TimerQueue>(4);
    {
        auto timer = std::make_shared<Timer::TimerQueue>(4);
        timer->Start();

        LOG_INFO("Now");
        // std::chrono::steady_clock::time_point tt = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        // timer->AddTaskAtTimePoint(tt, test_func2);
        auto dul = std::chrono::seconds(2);
        timer->AddRepeatTask(dul, 10, test_func2);
        // timer->AddTaskAfterDuration(dul, test_func2);
        // std::this_thread::sleep_for(std::chrono::seconds(3));
        timer->Shutdown();
    }
    // timer->Shutdown();
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // timer->AddTaskAfterDuration(std::chrono::duration<uint64_t, std::milli>(2000), test_func2);
    return 0;
}