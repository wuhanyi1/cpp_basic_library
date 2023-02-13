#include "common.h"

namespace why {

Semaphore::Semaphore(uint32_t val) {
    if (sem_init(&m_semaphore, 0, val)) {
        CHECK_THROW(false, "sem_init failed");
    }
}

Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}

void Semaphore::Wait() {
    int ret = sem_wait(&m_semaphore);
    CHECK_THROW(ret == 0, "sem_wait failed, ret is:%d", ret);
}

void Semaphore::Post() {
    int ret = sem_post(&m_semaphore);
    CHECK_THROW(ret == 0, "sem_post failed, ret is:%d", ret);
}

}