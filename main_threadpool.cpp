#include "threadpool.h"
#include "current_thread.h"
#include "count_down_latch.h"

#include <functional>
#include <stdio.h>
#include <unistd.h>

void print()
{
    printf("tid=%d\n", CurrentThread::tid());
}

void test(int maxSize)
{
    printf("Test ThreadPool with max queue size = %d\n", maxSize);
    ThreadPool pool("MainThreadPool");
    pool.setMaxQueueSize(maxSize);
    pool.start(3);

    printf("adding\n");
    pool.run(print);
    pool.run(print);
    pool.run(print);
    pool.run(print);
    pool.run(print);
    pool.run(print);
    pool.run(print);
    pool.run(print);
    pool.run(print);
    pool.run(print);
    pool.run(print);
    pool.run(print);
    printf("Done\n");

    CountDownLatch latch(1);
    pool.run(std::bind(&CountDownLatch::countDown, &latch));
    latch.wait();
    pool.stop();
}

int main()
{
    test(7);
    return 0;
}