#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include "noncopyable.h"
#include "http_request.h"
#include <unistd.h>
#include <cstring>
#include <vector>
#include "mutex.h"

#define MAXEVENTS   1024


class EPoll : public noncopyable
{
public:
    typedef std::vector<struct epoll_event> EventList;

    EPoll(int flags);
    ~EPoll();

    int epoll_add(int fd, HttpRequest* request, int events);
    int epoll_mod(int fd, HttpRequest* request, int events);
    int epoll_del(int fd, HttpRequest* request, int events);
    int epoll_wait(int max_events, int timeout_ms);
    //epoll_hander

    int get_epoll_fd(void) { return epoll_fd_; }
    int get_epoll_num_events(void) { return num_events_; }
    EventList& get_epoll_events(void) { return events_; }

private:
    int epoll_fd_;
    EventList events_;
    int num_events_;
    MutexLock mutex_;
};

#endif