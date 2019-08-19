#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include "noncopyable.h"
#include <unistd.h>
#include <cstring>
#include <vector>

#define MAXEVENTS   1024

//temp define;
class http_request
{
public:
    http_request(int num) { num_ = num; }
    int num_;
    //yj_timer* timer_;
};


class EPoll : public noncopyable
{
public:
    typedef std::vector<struct epoll_event> EventList;

    EPoll(int flags);
    ~EPoll();

    int epoll_add(int fd, http_request* request, int events);
    int epoll_mod(int fd, http_request* request, int events);
    int epoll_del(int fd, http_request* request, int events);
    int epoll_wait(int max_events, int timeout_ms);
    //epoll_hander

    int get_epoll_fd(void) { return epoll_fd_; }
    int get_epoll_num_events(void) { return num_events_; }
    EventList& get_epoll_events(void) { return events_; }

private:
    int epoll_fd_;
    EventList events_;
    int num_events_;
};

#endif