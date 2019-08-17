#ifndef CONDITION_H
#define CONDITION_H

#include "mutex.h"
#include "noncopyable.h"
#include <pthread.h>

class Condition : noncopyable
{
public:
    explicit Condition(MutexLock& mutex)
    : mutex_(mutex)
    {
        assert(pthread_cond_init(&pcond_, NULL));
    }
    ~Condition()
    {
        assert(pthread_cond_destroy(&pcond_));
    }

    void wait()
    {
        MutexLock::UnassignGuard ug(mutex_);
        assert(pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
    }

   // returns true if time out, false otherwise.
    bool waitForSeconds(double seconds);   

    void notify()
    {
        assert(pthread_cond_signal(&pcond_));
    }

    void notifyAll()
    {
        assert(pthread_cond_broadcast(&pcond_));
    }

private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};

#endif