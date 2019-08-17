#include "thread.h"
#include "current_thread.h"

#include <type_traits>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <assert.h>

namespace CurrentThread
{
  __thread int t_cachedTid = 0;
  __thread char t_tidString[32];
  __thread int t_tidStringLength = 6;
  __thread const char* t_threadName = "unknown";
  static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");
}

pid_t gettid()
{
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterFork()
{
  CurrentThread::t_cachedTid = 0;
  CurrentThread::t_threadName = "main";
  CurrentThread::tid();
  // no need to call pthread_atfork(NULL, NULL, &afterFork);
}

class ThreadNameInitializer
{
 public:
  ThreadNameInitializer()
  {
    CurrentThread::t_threadName = "main";
    CurrentThread::tid();
    pthread_atfork(NULL, NULL, &afterFork);
  }
};

ThreadNameInitializer init;

struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    string name_;
    std::weak_ptr<pid_t> wkTid_;

    ThreadData(const ThreadFunc& func,
                const string& name,
                const std::shared_ptr<pid_t>& tid)
    : func_(func),
        name_(name),
        wkTid_(tid)
        { }
    
    void runInThread()
    {
        pid_t tid = CurrentThread::tid();
        std::shared_ptr<pid_t> ptid = wkTid_.lock();
        if (ptid) {
            *ptid = tid;
            ptid.reset();
        }

        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        ::prctl(PR_SET_NAME, CurrentThread::t_threadName);
        try
        {
            func_();
            CurrentThread::t_threadName = "finished";
        }
        catch (...)
        {;}
    }
};

void* startThread(void* obj)
{
  ThreadData* data = static_cast<ThreadData*>(obj);
  data->runInThread();
  delete data;
  return NULL;
}

void CurrentThread::cacheTid()
{
  if (t_cachedTid == 0)
  {
    t_cachedTid = gettid();
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }    
}

bool CurrentThread::isMainThread()
{
  return tid() == ::getpid();
}

/* 
void CurrentThread::sleepUsec(int64_t usec)
{
  struct timespec ts = { 0, 0 };
  ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
  ::nanosleep(&ts, NULL);
}
*/
AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const string& n)
    : started_(false),
        joined_(false),
        pthreadId_(0),
        tid_(new pid_t(0)),
        func_(std::move(func)),
        name_(n)
{ setDefaultName(); }

Thread::~Thread()
{
    if (started_ && !joined_) {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
  int num = numCreated_.incrementAndGet();
  if (name_.empty())
  {
    char buf[32];
    snprintf(buf, sizeof buf, "Thread%d", num);
    name_ = buf;
  }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, tid_);
    if (pthread_create(&pthreadId_, NULL, startThread, data)) {
        started_ = false;
        delete  data;

    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}

