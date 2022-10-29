/*
 * @Author: wuhanyi
 * @Date: 2022-04-29 16:19:45
 * @LastEditTime: 2022-05-05 20:32:14
 * @FilePath: /basic_library/src/common/include/common/timer.h
 * @Description: 
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */
#ifndef __WHY_TIMER_H__
#define __WHY_TIMER_H__

#include <functional>
#include <memory>
#include <set>
#include <stdint.h>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <condition_variable>
#include "threadpool.h"

namespace why {

namespace Timer {
    
class TimerQueue : public std::enable_shared_from_this<TimerQueue>, public Noncopyable {
public:
    using ptr = std::shared_ptr<TimerQueue>;

    template<typename T, typename R>
    using Duration = std::chrono::duration<T, R>;

    using TimePoint = std::chrono::steady_clock::time_point;
    using Func = std::function<void()>;
    // 定时器需要执行的任务
    struct Task {
        Task(Func&& func_, const TimePoint &time_) : func(func_), timepoint(time_) {}
        Func func;
        TimePoint timepoint;
        bool operator<(const Task &t) const { return timepoint < t.timepoint; }
    };
    using TaskPtr = std::shared_ptr<Task>;

    TimerQueue(int pool_size = 4);

    ~TimerQueue() = default;

    void Start();

    void Shutdown();

    template<typename F, typename ...Args>
    void AddTaskAtTimePoint(const TimePoint& timepoint, F&& func, Args&& ...agrs) {
        Func f = std::bind(std::forward<F>(func), std::forward<Args>(agrs)...);
        TaskPtr task = std::make_shared<Task>(std::move(f), timepoint);
        std::unique_lock<std::mutex> lock(m_taskMtx);
        m_tasks.insert(task);
        lock.unlock();
        // 这里实际上只会有一个线程阻塞在这个条件变量上面
        m_cv.notify_all();
    }

    template<typename T, typename R, typename F, typename ...Args>
    void AddTaskAfterDuration(Duration<T, R> &duration, F&& func, Args&& ...args) {
        TimePoint timepoint = duration + std::chrono::steady_clock::now();
        AddTaskAtTimePoint(timepoint, std::forward<F>(func), std::forward<Args>(args)...);
    }

    template<typename T, typename R, typename F, typename ...Args>
    void AddRepeatTask(Duration<T, R> &interval, size_t repeat_num, F&& func, Args&& ...args) {
        TimePoint timepoint = std::chrono::steady_clock::now();
        for (size_t i = 0; i < repeat_num; i++) {
            timepoint += interval;
            AddTaskAtTimePoint(timepoint, std::forward<F>(func), std::forward<Args>(args)...);
        }
    }

private:
    // demon 线程会执行这个函数
    // 传给 demon 线程时，这里要用 enable_for_this,因为这是 demon 线程执行的函数,不受控制
    void LocalRun();

    struct cmp {
        bool operator()(const TaskPtr t1, const TaskPtr t2) {
            return t1->timepoint < t2->timepoint;
        }
    };

private:
    std::mutex m_taskMtx;
    std::set<TaskPtr, cmp> m_tasks; 
    ThreadPool m_threadPool;
    std::condition_variable m_cv;

    bool m_isTerminate{false};
};
}

}

#endif