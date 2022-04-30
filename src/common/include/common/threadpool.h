/*
 * @Author: wuhanyi
 * @Date: 2022-04-29 21:29:37
 * @LastEditTime: 2022-04-30 16:19:11
 * @FilePath: /basic_library/src/common/include/common/threadpool.h
 * @Description: 线程池
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */
#ifndef __WHY_THREADPOOL_H__
#define __WHY_THREADPOOL_H__

#include <thread>
#include <list>
#include <memory>
#include <mutex>
#include <vector>
#include <functional>
#include <future>
#include <condition_variable>

namespace why {

class ThreadPool {
public:
    using ThreadPtr = std::shared_ptr<std::thread>;
    struct Task {
        Task(std::function<void()> func_) : func(func_) {}
        std::function<void()> func;
    };
    using TaskPtr = std::shared_ptr<Task>;
    
    ThreadPool(int size = 0);

    void Start();

    template<typename Func, typename ...Args>
    auto execute(Func&& func, Args&& ...args) {
        using RetType = decltype(func(std::forward<Args>(args)...));

        auto pt = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        TaskPtr task = std::make_shared<Task>([pt] {
            (*pt)();
        });

        // 放入任务
        std::unique_lock<std::mutex> lock(m_taskMtx);
        m_taskLists.push_back(task);
        lock.unlock();

        // 唤醒一个线程
        m_cv.notify_one();

        return pt->get_future();
    }

    void Shutdown();

private:

    // 工作线程的执行函数
    void Run();
    
    TaskPtr GetTask();

private:
    std::mutex m_threadMtx;
    std::mutex m_taskMtx;
    size_t m_threadNum;
    std::vector<ThreadPtr> m_threads;
    std::list<TaskPtr> m_taskLists;

    std::condition_variable m_cv;

    bool m_isTerminate{false};

};

}

#endif