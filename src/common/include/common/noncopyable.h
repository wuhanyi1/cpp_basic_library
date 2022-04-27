/*
 * @Author: wuhanyi
 * @Date: 2022-04-26 14:28:20
 * @LastEditTime: 2022-04-26 14:43:15
 * @FilePath: /basic_library/src/common/include/noncopyable.h
 * @Description: 
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */
#ifndef __WHY_NONCOPYABLE_H__
#define __WHY_NONCOPYABLE_H__
namespace why {

class Noncopyable {
public:
    Noncopyable(const Noncopyable&) = delete;
    void operator=(const Noncopyable&) = delete;

    Noncopyable() = default;
    ~Noncopyable() = default;
};


}

#endif