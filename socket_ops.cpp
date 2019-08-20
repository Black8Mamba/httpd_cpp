#include "socket_ops.h"

#include <errno.h>
#include <fcntl.h>
#include <cstdio>  // snprintf
#include <strings.h>  // bzero
#include <sys/socket.h>
#include <unistd.h>
#include <sys/uio.h>


typedef struct sockaddr SA;

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in6* addr)
{
    return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in6* addr)
{
    return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in* addr)
{
    return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
}

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr)
{
    return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr)
{
    return static_cast<const struct sockaddr_in*>(implicit_cast<const void*>(addr));
}

const struct sockaddr_in6* sockets::sockaddr_in6_cast(const struct sockaddr* addr)
{
    return static_cast<const struct sockaddr_in6*>(implicit_cast<const void*>(addr));
}

int sockets::create_nonblock_socket(sa_family_t family)
{
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 
                IPPROTO_TCP);
    if (sockfd < 0)
        perror("sockets::create_nonblock_socket failed!\n");

    assert(sockfd > 0);
    return sockfd;    
}

int sockets::bind(int sockfd, const struct sockaddr* addr)
{
    int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));//6?
    if (ret < 0)
        perror("sockets::bind failed!\n");
    assert(ret == 0);
    return ret;
}

int sockets::listen(int sockfd)
{
    int ret = ::listen(sockfd, MAXCONN);
    if (ret < 0)
        perror("sockets::listen failed!\n");
    
    assert(ret == 0);
    return ret;
}

int sockets::accept(int sockfd, struct sockaddr_in* addr)
{
    socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));
    int connfd = ::accept4(sockfd, sockaddr_cast(addr),
                    &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);

    if (connfd < 0)
        perror("sockets::accept failed!\n");

    assert(connfd > 0);
    return connfd;
}

/*
int sockets::connect(int sockfd, const struct sockaddr* addr)
{
  return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}
*/

ssize_t sockets::read(int sockfd, void* buf, size_t count)
{
    size_t nleft = count;
    ssize_t nread;
    char *bufp = static_cast<char*>(buf);
    while(nleft > 0) {
        if ((nread = ::read(sockfd, bufp, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        } else if (nread == 0)
            break;
        nleft -= nread;
        bufp += nread;
    }
    return (count - nleft);
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt)
{
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count)
{
  size_t nleft = count;
  ssize_t nwrite;
  const char *bufp = static_cast<const char*>(buf);

  while(nleft > 0) {
      if ((nwrite = ::write(sockfd, bufp, nleft)) <= 0) {
          if (errno == EINTR)
            nwrite = 0;
          else
            return -1;
      }
      nleft -= nwrite;
      bufp += nwrite;
  }
    return (count - nleft);
}

void sockets::close(int sockfd)
{
    if (::close(sockfd) < 0)
        perror("sockets::close failed!\n");
}

void sockets::shutdown_write(int sockfd)
{
    if (::shutdown(sockfd, SHUT_WR) < 0)
        perror("sockets::shutdown_write failed!\n");
}

void sockets::to_ip_port(char* buf, size_t size, 
                            const struct sockaddr* addr)
{
    ;
}

void sockets::to_ip(char *buf, size_t size, 
                        const struct sockaddr* addr)
{
    ;
}

void sockets::from_ip_port(const char* ip, uint16_t port,
                        struct sockaddr_in* addr)
{
    ;
}

uint32_t sockets::hostToNet32(uint32_t host32) 
{ 
    return htonl(host32); 
}
uint16_t sockets::hostToNet16(uint16_t host16) 
{ 
    return htons(host16); 
}
uint32_t sockets::netToHost32(uint32_t net32) 
{ 
    return ntohl(net32);
 }
uint16_t sockets::netToHost16(uint16_t net16) 
{ 
    return ntohs(net16);
 }
