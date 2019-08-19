#include "noncopyable.h"

// struct tcp_info is in <netinet/tcp.h>
//struct tcp_info;

class InetAddress;

class Socket:noncopyable
{
public:
    explicit Socket(int sockfd)
        : sockfd_(sockfd)
        { }
    
    ~Socket();

    int fd(void) const { return sockfd_; }
    //bool getTcpInfo(struct tcp_info*) const;
    //bool getTcpInfoString(char* buf, int len) const;
    void bindAddress(const InetAddress& localaddr);
    void listen(void);
    int accept(InetAddress* peeraddr);
    void shutdownWrite(void);

    /// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
    ///
    void setTcpNoDelay(bool on); //Nagle

    /// Enable/disable SO_REUSEADDR
    ///
    void setReuseAddr(bool on);   

    /// Enable/disable SO_REUSEPORT
    ///
    void setReusePort(bool on);

    /// Enable/disable SO_KEEPALIVE
    ///
    void setKeepAlive(bool on);

private:
    const int sockfd_;
};