#ifndef __WHY_INET_ADDRESS_H__
#define __WHY_INET_ADDRESS_H__
#include <stdint.h>
#include <string>
#include <arpa/inet.h>
#include "common.h"

namespace why {
class InetAddress : public Noncopyable {
public:
    InetAddress(uint16_t _port, const std::string& _ip);
    
    uint16_t GetPort() const { return m_port; }
    const std::string& GetStrIp() const { return m_ip; }
    struct sockaddr_in GetAddr() const { return m_addr; }

private:

    uint16_t m_port;
    const std::string m_ip;
    struct sockaddr_in m_addr;
};

}



#endif