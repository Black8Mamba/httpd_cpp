#include "threadpool.h"

#include <algorithm>
#include <functional>
#include <assert.h>
#include <stdio.h>

ThreadPool::ThreadPool(const string& nameArg)
    : mutex_(),
      notEmpty_(mutex_),
      notFull_(mutex_),
      name_(nameArg),
      maxQueueSize_(0),
      running_(false)
{

}

ThreadPool::~ThreadPool()
{
    if (running_) {
        stop();
    }
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i) {
        char id[32];
        snprintf(id, sizeof(id), "%d", i+1);
        threads_.push_back(std::unique_ptr<Thread>(new Thread(
            std::bind(&ThreadPool::runInThread, this), name_+id)));
        threads_[i]->start();
    }
    if (numThreads == 0 && threadInitCallback_) {
        threadInitCallback_();
    }
}

void ThreadPool::stop()
{
    {
    MutexLockGuard lock(mutex_);
    running_ = false;
    notEmpty_.notifyAll();
    }

  std::for_each(threads_.begin(),
           threads_.end(),
           std::bind(&Thread::join, std::placeholders::_1));   
}

size_t ThreadPool::queueSize() const
{
    MutexLockGuard lock(mutex_);
    return queue_.size();
}

void ThreadPool::run(const Task& task) //put a task
{
    if (threads_.empty()) {
        task();//工作线程为空，主线程跑？
    } else {
        MutexLockGuard lock(mutex_);
        while(isFull()) {
            notFull_.wait(); //等待不满的条件
        }
        assert(!isFull()); //确定不满

        queue_.push_back(task);
        notEmpty_.notify(); //通知不空；
    }
}

/*
void ThreadPool::run(Task&& task)
{
  if (threads_.empty())
  {
    task();
  }
  else
  {
    MutexLockGuard lock(mutex_);
    while (isFull())
    {
      notFull_.wait();
    }
    assert(!isFull());

    queue_.push_back(std::move(task));
    notEmpty_.notify();
  }
}
*/

ThreadPool::Task ThreadPool::take()
{
    MutexLockGuard lock(mutex_);
    while(queue_.empty() && running_) {
        notEmpty_.wait(); //等待不空
    }

    Task task;
    if (!queue_.empty()) {
        task = queue_.front();
        queue_.pop_front();
        if (maxQueueSize_ > 0) {
            notFull_.notify();
        }
    }

    return task;
}

bool ThreadPool::isFull() const 
{
    mutex_.assertLocked(); //判断是否已上锁
    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_; 
}

void ThreadPool::runInThread() //没有加异常捕获
{
    if (threadInitCallback_) {
        threadInitCallback_();
    }
    while(running_) {
        Task task(take());
        if (task) {
            task();
        }
    }
}