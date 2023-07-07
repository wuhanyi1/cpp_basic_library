/*
 * @Author: wuhanyi1 874864297@qq.com
 * @Date: 2023-03-21 19:42:01
 * @LastEditors: wuhanyi1 874864297@qq.com
 * @LastEditTime: 2023-05-02 13:40:33
 * @FilePath: /cpp_basic_library/src/net/Channel.h
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */

#ifndef __WHY_CHANNEL_H__
#define __WHY_CHANNEL_H__

#include "noncopyable.h"
#include <functional>

namespace why {
namespace net {

class EventLoop;
class Channel : public Noncopyable {
public:
    Channel(int fd) : m_fd(fd) {}
    ~Channel() = default;

    using WriteEventCb = std::function<void()>;
    using ReadEventCb = std::function<void(unsigned long long)>;

    void SetWriteEventCb(WriteEventCb& cb) { m_writeCb = cb; }
    void SetWriteEventCb(WriteEventCb&& cb) { m_writeCb = std::move(cb); }

    void SetReadEventCb(ReadEventCb& cb) { m_readCb = cb; }
    void SetReadEventCb(ReadEventCb&& cb) { m_readCb = std::move(cb); }

    void HandleEvent();

private:
    const int m_fd;
    WriteEventCb m_writeCb{nullptr};
    ReadEventCb m_readCb{nullptr};
};


}

}

#endif