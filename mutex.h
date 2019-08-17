#ifndef MUTEX_H
#define MUTEX_H

#include "current_thread.h"
#include "noncopyable.h"
#include "assert.h"
#include <pthread.h>

class MutexLock : noncopyable
{
public:
    MutexLock() : holder_(0)
    {
        assert(pthread_mutex_init(&mutex_, NULL) == 0);
    }

    ~MutexLock()
    {
        assert(holder_ == 0);
        assert(pthread_mutex_destroy(&mutex_));
    }

    bool isLockedByThisThread() const
    {
        return holder_ == CurrentThread::tid();
    }

    void assertLocked() const
    {
        assert(isLockedByThisThread());
    }

    // internal usage
    void lock()
    {
        assert(pthread_mutex_lock(&mutex_));
        assignHolder();
    }

    void unlock()
    {
        unassignHolder();
        assert(pthread_mutex_unlock(&mutex_));
    }

    pthread_mutex_t* getPthreadMutex()
    {
        return &mutex_;
    }

private:
   friend class Condition; 
    class UnassignGuard : noncopyable
    {
    public:
        UnassignGuard(MutexLock& owner)
        : owner_(owner)
        {
        owner_.unassignHolder();
        }

        ~UnassignGuard()
        {
        owner_.assignHolder();
        }

    private:
        MutexLock& owner_;
    };

    void unassignHolder()
    {
        holder_ = 0;
    }

  void assignHolder()
    {
        holder_ = CurrentThread::tid();
    }

    pthread_mutex_t mutex_;
    pid_t holder_;
};

class MutexLockGuard : noncopyable
{
public:
    explicit MutexLockGuard(MutexLock& mutex)
    : mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock &mutex_;
};

#endif