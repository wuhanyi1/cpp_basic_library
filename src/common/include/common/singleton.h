/*
 * @Author: wuhanyi
 * @Date: 2022-04-26 20:03:15
 * @LastEditTime: 2022-04-26 20:11:24
 * @FilePath: /basic_library/src/common/include/common/singleton.h
 * @Description: 
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */
#ifndef __WHY_SINGLETON_H__
#define __WHY_SINGLETON_H__

#include "noncopyable.h"

namespace why {

/**
 * @description: 单例模式封装类
 * @details T 类型
 *          X 为了创造多个实例对应的Tag
 *          N 同一个Tag创造多个实例索引
 */
template<typename T, class X = void, int N = 0>
class Singleton : Noncopyable {
public:
    static T& Instance() {
        static T val;
        return val;
    }
};

}

#endif
