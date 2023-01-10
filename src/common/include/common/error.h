/*
 * @Author: wuhanyi
 * @Date: 2022-04-26 15:08:04
 * @LastEditTime: 2023-01-08 11:48:35
 * @FilePath: /cpp_basic_library/src/common/include/common/error.h
 * @Description: 
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */
#ifndef __WHY_ERROR_H__
#define __WHY_ERROR_H__

namespace why {

// #ifndef ENABLE_EXCEPTION
#define THROW(e) throw(e)
// #else
// #define THROW(e) std::abort()
// #endif

#ifdef ENABLE_GCC_BUILTIN
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

#define CHECK_THROW(cond, ...)                          \
    do {                                                \
        if (LIKELY(cond) == 0) {                        \
            THROW(Exception(__VA_ARGS__));              \
        }                                               \
    } while(0)                              

}

#endif