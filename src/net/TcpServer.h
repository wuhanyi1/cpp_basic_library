#ifndef __WHY_TCP_SERVER_H__
#define __WHY_TCP_SERVER_H__

#include "InetAddress.h"
#include "common.h"
namespace why {
class Acceptor;
class Channel;

class TcpServer :: public Noncopyable {
public:
    TcpServer(InetAddress addr);

private:
    Channel m_listenChannel;
    Acceptor m_acceptor;
    EventLoop* m_loop; // acceptor 使用的 eventloop
    // 一个 EventLoop 数据，每个 EventLoop 由一个专门的线程处理
};

}

#endif