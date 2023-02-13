/*
 * @Author: wuhanyi
 * @Date: 2022-04-26 14:41:55
 * @LastEditTime: 2023-01-12 14:37:48
 * @FilePath: /cpp_basic_library/src/common/include/common/exception.h
 * @Description: 
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */
#ifndef __WHY_EXCEPTION_H__
#define __WHY_EXCEPTION_H__

#include <exception>
#include <iostream>
#include <stdarg.h>

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


class Exception : public std::exception {
public:    
    Exception() = default;
    Exception(const char *fmt = "", ...) {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(&buffer_[0], kBufferSize - 1, fmt, ap);
        va_end(ap);
    }

    const char *what() const noexcept override { return buffer_; }

private:
    static constexpr size_t kBufferSize{2048U};
    char buffer_[kBufferSize]{};
};
}

#endif