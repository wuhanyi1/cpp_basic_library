#include "log.h"
#include "common.h"
#include <thread>
#include <chrono>
using namespace why;

void test_func() {
    LOG_INFO("test_func:%s", "1111");
}

void test_func2() {
    LOG_INFO("test_func2:%s", "1111");
}

int main() {
    // int n = 1e5;
    // auto p1 = std::chrono::steady_clock::now().time_since_epoch();
    // while (n--) {
    //     LOG_INFO("第一次测试：%s", "111111111111111111111111");
    // }
    // auto p2 = std::chrono::steady_clock::now().time_since_epoch();;
    // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(p2 - p1).count() << std::endl;

    why::ThreadPool pool;
    pool.Start();
    pool.execute(test_func);
    pool.execute(test_func2);
    pool.Shutdown();
    return 0;
}