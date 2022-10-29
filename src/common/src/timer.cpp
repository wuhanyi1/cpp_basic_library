/*
 * @Author: wuhanyi
 * @Date: 2022-04-29 16:21:56
 * @LastEditTime: 2022-05-04 14:06:46
 * @FilePath: /basic_library/src/common/src/timer.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */
#include "common.h"

namespace why {
using namespace Timer;

TimerQueue::TimerQueue(int pool_size) : m_threadPool(pool_size) {
    
}

void TimerQueue::Start() {
    m_threadPool.Start();
    std::thread(&TimerQueue::LocalRun, shared_from_this()).detach();
}

void TimerQueue::Shutdown() {
    std::unique_lock<std::mutex> lock(m_taskMtx);
    m_cv.wait(lock, [this] {
        return m_tasks.empty();
    });
    CHECK_THROW(m_tasks.empty(), "m_task not empty");
    CHECK_THROW((m_isTerminate == false), "TimerQueue has already shutdown");
    m_isTerminate = true;
    // 唤醒 demon 线程
    m_cv.notify_all();
    m_threadPool.Shutdown();
}

void TimerQueue::LocalRun() {
    while(!m_isTerminate) {
        std::unique_lock<std::mutex> lock(m_taskMtx);
        m_cv.wait(lock, [this] {
            return !m_tasks.empty() || m_isTerminate;
        });
        if (m_isTerminate) {
            return;
        }
        CHECK_THROW((m_tasks.empty() == false), "m_task is empty");
        if (!m_tasks.empty()) {
            auto now = std::chrono::steady_clock::now();
            for (auto task = m_tasks.begin(); task != m_tasks.end(); task++) {
                auto diff = (*task)->timepoint - now;
                auto taskptr = *task;
                if (std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() > 0) {
                    // std::cout << std::chrono::duration_cast<std::chrono::seconds>(diff).count() << std::endl;
                    m_threadPool.execute([taskptr] {
                        std::this_thread::sleep_until(taskptr->timepoint);
                        taskptr->func();
                    });
                }
            }
            m_tasks.clear();
        }
        
        // 唤醒可能因为调用 Shutdown 阻塞在 m_cv 上的线程
        m_cv.notify_all();
    }
}

}