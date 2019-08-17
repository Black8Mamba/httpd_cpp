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
#include "timer.h"

using std::priority_queue;

class yj_priority_queue
{
public:
    yj_priority_queue() { yj_pq_.resize(10); }
    bool yj_pq_is_empty(void);
    size_t yj_pq_size(void);
    yj_timer* yj_pq_min(void);
    int yj_pq_delmin(void);
    int yj_pq_insert(yj_timer*);

private:
    priority_queue<yj_timer*, vector<yj_timer*>, less<int>>  yj_pq_;//自己定义less
};

