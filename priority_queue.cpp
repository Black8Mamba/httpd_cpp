/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-08-17 13:00:45
 * @LastEditTime: 2019-08-17 13:03:11
 * @LastEditors: Please set LastEditors
 */

#include "priority_queue.h"

void yj_priority_queue::add_timer(http_request* request, 
                                  size_t timeout, 
                                function<int(http_request*)> func)
{
    //申请timer节点
    yj_timer* timer_node = new yj_timer();
    //request->timer = timer_node;
    request->timer_ = timer_node;
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

void yj_priority_queue::del_timer(http_request* request)
{
    //更新当前时间
    yj_timer::time_update();   
    yj_timer* timer_node = request->timer_;
    timer_node->set_deleted(true);
}

void yj_priority_queue::handle_expire_timers(void)
{
    while(!is_empty()) {    
        yj_timer* timer_node = min(); //获取最小节点
        if (timer_node->is_deleted()) { //如果被标记为删除
            delmin(); //pop
            delete timer_node; //释放空间
            continue;
        }
        //更新当前时间
        yj_timer::time_update(); 
        if (timer_node->get_key() > yj_timer::current_msec) {
            return; //未超时
        }

        //超时未被删除 调用handler处理
        if (timer_node->get_func()) {
            function<int(http_request*)> func = timer_node->get_func();//考虑指针优化
            func(timer_node->get_request());
        }

        delmin();
        delete timer_node;
    }
}
