#ifndef SOCKET_OPS_H
#define SOCKET_OPS_H

#include <arpa/inet.h>
#include <assert.h>

#define MAXCONN   1024

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

namespace sockets
{

int create_nonblock_socket(sa_family_t family);
//int connect(int sockfd, const struct sockaddr* addr);
int bind(int sockfd, const struct sockaddr* addr);
int listen(int sockfd);
int accept(int sockfd, struct sockaddr_in* addr);
ssize_t read(int sockfd, void *buf, size_t count);
ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);
ssize_t write(int sockfd, const void *buf, size_t count);
void close(int sockfd);
void shutdown_write(int sockfd);

void to_ip_port(char* buf, size_t size, const struct sockaddr* addr);
void to_ip(char* buf, size_t size, const struct sockaddr* addr);

void from_ip_port(const char* ip, uint16_t port, struct sockaddr_in* addr);
//void from_ip_port(const char* ip, uint16_t port, struct sockaddr_in6* addr);

//int getSocketError(int sockfd);

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);
struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);

const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);

//struct sockaddr_in6 getLocalAddr(int sockfd);
//struct sockaddr_in6 getPeerAddr(int sockfd);
//bool isSelfConnect(int sockfd);

uint32_t hostToNet32(uint32_t host32);
uint16_t hostToNet16(uint16_t host16);
uint32_t netToHost32(uint32_t net32);
uint16_t netToHost16(uint16_t net16);

}

#endif