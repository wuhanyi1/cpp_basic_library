/*
 * @Author: wuhanyi
 * @Date: 2022-04-26 14:41:55
 * @LastEditTime: 2022-04-26 15:08:43
 * @FilePath: /basic_library/src/common/include/exception.h
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