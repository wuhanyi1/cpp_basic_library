/*
 * @Author: wuhanyi1 874864297@qq.com
 * @Date: 2023-02-09 13:39:15
 * @LastEditors: wuhanyi1 874864297@qq.com
 * @LastEditTime: 2023-02-09 13:43:07
 * @FilePath: /cpp_basic_library/src/common/include/common/mutex.h
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __WHY_MUTEX_H__
#define __WHY_MUTEX_H__

#include <semaphore.h>
#include <stdint.h>

namespace why {

class Semaphore {
public:
    Semaphore(uint32_t val = 0);
    ~Semaphore();

    void Wait();

    void Post();

private:
    sem_t m_semaphore;
};

}

#endif