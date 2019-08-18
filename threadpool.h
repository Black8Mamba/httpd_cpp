#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "condition.h"
#include "mutex.h"
#include "thread.h"
#include "noncopyable.h"

#include <functional>
#include <vector>
#include <deque>

using std::vector;
using std::deque;

class ThreadPool : noncopyable
{
public:
    typedef std::function<void()> Task;
    explicit ThreadPool(const string& nameArg = string("ThreadPool"));
    ~ThreadPool();

    void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
    void setThreadInitCallback(const Task& cb) { threadInitCallback_ = cb; }

    void start(int numThreads);
    void stop();

    const string& name() const { return name_; }
    size_t queueSize() const;

    void run(const Task& f);
    //void run(const Task&& f);

private:

    bool isFull() const;
    void runInThread();
    Task take();

    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    string name_;
    Task threadInitCallback_;
    vector<std::unique_ptr<Thread>> threads_;
    deque<Task> queue_;
    size_t maxQueueSize_;
    bool running_;
};

#endif