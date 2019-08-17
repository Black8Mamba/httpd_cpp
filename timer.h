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

#define TIMEOUT_DEFAULT 500 //ms

class yj_timer;

//temp define;
class http_request
{
public:
    http_request(int num) { num_ = num; timer_ = NULL; }
    int num_;
    yj_timer* timer_;
};

class yj_timer
{
public:
    yj_timer() { deleted_ = 0; key_ = 0; }  //需要更新当前时间吗？
    void set_key(size_t key) { key_ = key; }
    void set_deleted(bool deleted) { deleted_ = deleted; }
    void set_handler(std::function<int(http_request*)> func) { func_ = func; }
    void set_request(http_request* request) { request_ = request; }

    static void time_update();

    size_t get_key(void) { return key_; }
    bool is_deleted(void) { return deleted_; }
    http_request* get_request(void) { return request_; } 
    std::function<int(http_request*)> get_func(void) { return func_; }

    static size_t current_msec;//当前时间

private:
    size_t key_;  // 标记超时时间
    bool deleted_; // 标记是否被删除
    std::function<int(http_request*)> func_; // 超时处理，add时指定
    http_request* request_; //http请求
};

#endif