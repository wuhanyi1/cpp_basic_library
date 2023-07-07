#include "InetAddress.h"

namespace why {

InetAddress::InetAddress(uint16_t _port, const std::string& _ip) : m_port(_port), m_ip(_ip) {
    if (inet_pton(AF_INET, _ip.c_str(), &m_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
    }
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(_port);
}


}