/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-17 11:32:39
 * @LastEditTime: 2019-08-17 13:03:50
 * @LastEditors: Please set LastEditors
 */
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <queue>
#include <vector>
#include "timer.h"
#include <functional>

using std::priority_queue;
using std::vector;
using std::function;

class HttpRequest;

struct timer_comp //重写仿函数
{
    bool operator() (yj_timer* a, yj_timer* b) 
    {
        return a->get_key() > b->get_key();
    }
};

class yj_priority_queue
{
public:
    yj_priority_queue() { ; }

    void add_timer(HttpRequest* request, size_t timeout, 
                    function<int(HttpRequest*)> func); //添加定时器
    void del_timer(HttpRequest* request);  //删除定时器
    void handle_expire_timers(void);  //超时处理
    size_t size(void) { return yj_pq_.size(); }


public:
    bool is_empty(void) { return yj_pq_.empty(); }
    
    yj_timer* min(void) { return yj_pq_.top(); }
    void delmin(void) {  yj_pq_.pop(); }
    void insert(yj_timer* timer) { yj_pq_.push(timer); }

private:
    priority_queue<yj_timer*, vector<yj_timer*>, timer_comp>  yj_pq_;
    MutexLock mutex_;
};

#endif