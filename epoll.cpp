#include "epoll.h"

EPoll::EPoll(int flags)
{
    epoll_fd_ = ::epoll_create1(flags);
    if (epoll_fd_ < 0) {
        throw "EPoll create failed";
    }

    events_.resize(MAXEVENTS);
    num_events_ = 0;
}

EPoll::~EPoll()
{
    ::close(epoll_fd_);
}

int EPoll::epoll_add(int fd, HttpRequest* request, int events)
{
   // MutexLockGuard guard(mutex_);
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.ptr = static_cast<void*>(request);
    event.events = events;
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
    
    return ret;
}

int EPoll::epoll_mod(int fd, HttpRequest* request, int events)
{
   // MutexLockGuard guard(mutex_);
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.ptr = static_cast<void*>(request);
    event.events = events;
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event);
    
    return ret;
}

int EPoll::epoll_del(int fd, HttpRequest* request, int events)
{
    //MutexLockGuard guard(mutex_);
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.ptr = static_cast<void*>(request);
    event.events = events;
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &event);
    
    return ret;
}

int EPoll::epoll_wait(int max_events, int timeout_ms)
{
//    MutexLockGuard guard(mutex_);
    int num_events = ::epoll_wait(epoll_fd_, 
                                    &*events_.begin(),
                                    static_cast<int>(events_.size()),
                                    timeout_ms);
    this->num_events_ = num_events;
    return num_events;
}