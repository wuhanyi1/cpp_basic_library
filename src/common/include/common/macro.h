#ifndef __MACRO_H__
#define __MACRO_H__

#include "exception.h"
#include "utils.h"
#include <assert.h>
#include <sstream>

// #ifndef ENABLE_EXCEPTION
#define THROW(e) throw(e)
// #else
// #define THROW(e) std::abort()
// #endif

#if defined __GNUC__ || defined __llvm__
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

#define CHECK_THROW(cond, ...)                          \
    do {                                                \
        if (LIKELY(cond) == 0) {                        \
            THROW(why::Exception(__VA_ARGS__));              \
        }                                               \
    } while(0)                       

#define LOCK_GUARD std::lock_guard<std::mutex>
#define UNIQUE_LOCK std::unique_lock<std::mutex>


// 断言宏封装
#define ASSERT(x) \
    if(UNLIKELY(!(x))) { \
        std::stringstream ss{}; \
        ss << "[thread: " << why::ThisThread::GetName().c_str() << "] ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << why::BacktraceToString(100, 2, "    "); \
        std::cout << ss.str() << std::endl;\
        assert(x); \
    }

// 断言宏封装
#define ASSERT2(x, w) \
    if(UNLIKELY(!(x))) { \
        std::stringstream ss{}; \
        ss << "[thread: " << why::ThisThread::GetName().c_str() << "] ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << sylar::BacktraceToString(100, 2, "    "); \
        std::cout << ss.str() << std::endl;     \
        assert(x); \
    }

#endif