#include "common.h"

namespace why {

static thread_local std::string t_thread_name = "UNKNOWN";
static thread_local Thread* t_thread = nullptr;

void Thread::Join() {
    if (m_thread) {
        int ret = pthread_join(m_thread, nullptr);
        CHECK_THROW(ret == 0, "pthread_join failed, ret is:%d", ret);
    }
}

void Thread::Detach() {
    if (m_thread) {
        int ret = pthread_detach(m_thread);
        CHECK_THROW(ret == 0, "pthread_detach failed, ret is:%d", ret);
    }
}

void* Thread::run(void* arg) {
    Thread* t = reinterpret_cast<Thread*>(arg);
    t_thread = t;
    t->m_id = gettid();
    if (!t->m_name.empty()) {
        t_thread_name = t->m_name;
        int ret = pthread_setname_np(t->m_thread, t->m_name.c_str());
        CHECK_THROW(ret == 0, "pthread_setname_np failed, ret is:%d", ret);
    }
    auto cb = std::move(t->m_cb);
    t->m_semaphore.Post();
    cb();
    return nullptr;
}

const std::string& ThisThread::GetName() {
    return t_thread_name;
}

void ThisThread::SetName(const std::string& name) {
    t_thread_name = name;
    int ret{};
    if (t_thread) {
        t_thread->m_name = t_thread_name;
        ret = pthread_setname_np(t_thread->m_thread, t_thread_name.c_str());
    }
    ret = pthread_setname_np(pthread_self(), t_thread_name.c_str());
    CHECK_THROW(ret == 0, "pthread_setname_np failed, ret is:%d", ret);
}

pid_t ThisThread::GetID() {
    if (t_thread) {
        return t_thread->m_id;
    } else {
        return gettid();
    }
}

}