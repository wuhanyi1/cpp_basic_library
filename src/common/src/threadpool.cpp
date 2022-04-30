#include "common.h"

namespace why {

ThreadPool::ThreadPool(int size) : m_threadNum(size) {
    if (!size) {
        m_threadNum = std::thread::hardware_concurrency();
    }
}

void ThreadPool::Start() {
    std::lock_guard<std::mutex> lock(m_threadMtx);
    CHECK_THROW(m_threads.empty(), "m_threads not empty");
    for (size_t i = 0; i < m_threadNum; i++) {
        m_threads.push_back(std::make_shared<std::thread>(std::bind(&ThreadPool::Run, this)));
    }
}

void ThreadPool::Shutdown() {
    while (1) {
        std::unique_lock<std::mutex> lock(m_taskMtx);
        if (m_taskLists.empty()) {
            // lock.unlock();
            break;
        }
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::lock_guard<std::mutex> lock(m_threadMtx);
    m_isTerminate = true;
    m_cv.notify_all();
    for (auto thread : m_threads) {
        thread->join();
    }
}

ThreadPool::TaskPtr ThreadPool::GetTask() {
    std::unique_lock<std::mutex> lock(m_taskMtx);
    // 任务队列不为空或者线程池已经终止时才解除阻塞
    m_cv.wait(lock, [this] {
        return !m_taskLists.empty() || m_isTerminate;
    });
    if (m_isTerminate) {
        return nullptr;
    }
    CHECK_THROW((m_taskLists.empty() == false), "task list is empty!");
    auto task = m_taskLists.front();
    m_taskLists.pop_front();

    return task;
}

void ThreadPool::Run() {
    while (!m_isTerminate) {
        auto task = GetTask();
        if (task) {
            try {
                task->func();
            } catch (const std::exception& e) {
                CHECK_THROW(false, "task func exec error:%s", e.what());
            }
        }
    }
}

}