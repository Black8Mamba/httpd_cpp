#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

//#include "copyable.h" 
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>

using std::string;

namespace sockets
{
const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
}

class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0);
    InetAddress(string ip, uint16_t port);
    explicit InetAddress(const struct sockaddr_in& addr)
        : addr_(addr)
    { }

    sa_family_t family() const { return addr_.sin_family; }
   // string toIp() const;
   // string toIpPort() const;
   // uint16_t toPort() const;

    const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr_); }
    void setSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }

   // uint32_t ipNetEndian() const;
   // uint16_t portNetEndian() const { return addr_.sin_port; }

private:
    struct sockaddr_in addr_;
};

#endif