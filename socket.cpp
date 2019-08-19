#include "socket.h"
#include "inet_address.h"
#include "socket_ops.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>

Socket::~Socket()
{
    sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& addr)
{
    sockets::bind(sockfd_, addr.getSockAddr());
}

void Socket::listen()
{
    sockets::listen(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    int connfd = sockets::accept(sockfd_, &addr);
    if (connfd >= 0)
        peeraddr->setSockAddrInet(addr);

    return connfd;
}

void Socket::shutdownWrite()
{
    sockets::shutdown_write(sockfd_);
}

void Socket::setTcpNoDelay(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

void Socket::setReuseAddr(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK    
}

void Socket::setReusePort(bool on)
{
    ;
}

void Socket::setKeepAlive(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK    
}