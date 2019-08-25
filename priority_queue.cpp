/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-17 13:00:45
 * @LastEditTime: 2019-08-17 13:03:11
 * @LastEditors: Please set LastEditors
 */

#include "priority_queue.h"
#include "http_request.h"
#include "mutex.h"
#include <iostream>
using namespace std;

void yj_priority_queue::add_timer(HttpRequest* request, 
                                  size_t timeout, 
                                function<int(HttpRequest*)> func)
{
    MutexLockGuard guard(mutex_);
    cout << "enter add_timer" << endl;
    //申请timer节点
    yj_timer* timer_node = new yj_timer();
    timer_node->set_request(request);//timer需要request吗,需要的
    request->setTimer(timer_node);
    //更新当前时间
    yj_timer::time_update();
    // 加入时设置超时阈值，删除信息,handler等
    timer_node->set_key(yj_timer::current_msec + timeout);
    timer_node->set_deleted(false);
    timer_node->set_handler(func);
    timer_node->set_request(request);

    //新节点插入队列
    yj_priority_queue::insert(timer_node);
}

void yj_priority_queue::del_timer(HttpRequest* request)
{
    MutexLockGuard guard(mutex_);
    //更新当前时间
    yj_timer::time_update();   
    yj_timer* timer_node = request->getTimer();
    timer_node->set_deleted(true);
}

void yj_priority_queue::handle_expire_timers(void)
{
    MutexLockGuard guard(mutex_);
    cout << "enter handle_timers" << endl;
    while(!is_empty()) {    
        yj_timer* timer_node = min(); //获取最小节点
        if (timer_node->is_deleted()) { //如果被标记为删除
            delmin(); //pop
            cout << "111111" << endl;
            cout << timer_node << endl;
            delete timer_node; //释放空间
            continue;
        }
        //更新当前时间
        yj_timer::time_update(); 
        if (timer_node->get_key() >= yj_timer::current_msec) {
            return; //未超时
        } else {
            //超时未被标记删除 调用handler处理，关闭后
            if (timer_node->get_func()) {
                function<int(HttpRequest*)> func = timer_node->get_func();
                func(timer_node->get_request());
                delmin();
                cout << "33333333" << endl;
                cout << timer_node << endl;
                delete timer_node;
            }
        }
    }
}

