/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-17 12:36:38
 * @LastEditTime: 2019-08-17 12:52:51
 * @LastEditors: Please set LastEditors
 */
#ifndef JY_TIMER_H
#define JY_TIMER_H

#include <functional>
#include <iostream>
#include "mutex.h"

#define TIMEOUT_DEFAULT 500 //ms

class HttpRequest;

class yj_timer
{
public:
    yj_timer() 
        : deleted_(0),
            key_(0),
            request_(NULL)
     { }  //需要更新当前时间吗？
    void set_key(size_t key) { MutexLockGuard guard(mutex_); key_ = key; }
    void set_deleted(bool deleted) { MutexLockGuard guard(mutex_); deleted_ = deleted; }
    void set_handler(std::function<int(HttpRequest*)> func) { MutexLockGuard guard(mutex_); func_ = func; }
    void set_request(HttpRequest* request) { MutexLockGuard guard(mutex_); request_ = request; }

    static void time_update();

    size_t get_key(void) { return key_; }
    bool is_deleted(void) { return deleted_; }
    HttpRequest* get_request(void) { return request_; } 
    std::function<int(HttpRequest*)> get_func(void) { return func_; }

    static size_t current_msec;//当前时间

private:
    size_t key_;  // 标记超时时间
    bool deleted_; // 标记是否被删除
    std::function<int(HttpRequest*)> func_; // 超时处理，add时指定
    HttpRequest* request_; //http请求
    MutexLock mutex_;
};

#endif