#include "inet_address.h"
#include "socket_ops.h"
#include <netdb.h>
#include <strings.h>
#include <netinet/in.h>

InetAddress::InetAddress(uint16_t port)
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = sockets::hostToNet32(INADDR_ANY);
    addr_.sin_port = sockets::hostToNet16(port);
}

InetAddress::InetAddress(string ip, uint16_t port)
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = sockets::hostToNet16(port);
    if (::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0)
        perror("InetAddress::InetAddress::inet_pton failed!\n");
    
    throw("InetAddress failed!\n");
}