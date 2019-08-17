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

#define TIMEOUT_DEFAULT 500 //ms

static int timeout(int num)
{
    std::cout << "timeout: " << num << std::endl;
    return 0; 
}

class yj_timer
{
public:
    yj_timer() { deleted = 0; key = 0; }
    void set_key(size_t key) { key_ = key; }
    void set_deleted(bool deleted) { deleted_ = deleted; }
    void set_handler(function<int(int)> func) { func_ = func; }
    //void set_request(request_t request);

private:
    size_t key_;  // 标记超时时间
    bool deleted_; // 标记是否被删除
    function<int(int)> func_; // 超时处理，add时指定
    //request_t request_;
};

#endif