/*
 * @Author: wuhanyi1 874864297@qq.com
 * @Date: 2023-01-11 11:51:45
 * @LastEditors: wuhanyi1 874864297@qq.com
 * @LastEditTime: 2023-02-15 19:06:18
 * @FilePath: /cpp_basic_library/src/common/include/common/thread.h
 * @Description: 
 * 
 * Copyright (c) 2023 by wuhanyi1 874864297@qq.com, All Rights Reserved. 
 */
#ifndef __WHY_THREAD_H__
#define __WHY_THREAD_H__

#include <functional>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include "noncopyable.h"
#include "macro.h"
#include <condition_variable>
#include "utils.h"
#include "mutex.h"

namespace why {

class Thread : public Noncopyable {
friend class ThisThread;    
public:
    using Func = std::function<void()>;

    template<typename F, typename... Args>
    Thread(F&& func, Args... args, const std::string& name) : m_name(name) {
        m_cb = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
        int ret = pthread_create(&m_thread, nullptr, Thread::run, this);
        CHECK_THROW(ret == 0, "pthread_create failed, ret = %d", ret);
        m_semaphore.Wait();
    }
    
    pid_t GetPid() const { return m_id; }

    const std::string& GetName() const { return m_name; }

    void Join();
    void Detach();

    ~Thread() {
        if (m_thread) {
            int ret = pthread_detach(m_thread);
            (void)ret;
            // CHECK_THROW(ret == 0, "pthread_detach failed, ret is:%d", ret);
        }
    }

private:
    static void* run(void* arg);

private:
    pid_t m_id{0};// thread id
    pthread_t m_thread{0};
    Func m_cb;
    std::string m_name;
    Semaphore m_semaphore;
};

class ThisThread {
public:
    
    static const std::string& GetName();
    static void SetName(const std::string& name);
    static pid_t GetID();
};

}

#endif