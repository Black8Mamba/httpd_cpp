#include "callback.h"
#include "http_request.h"
#include "http_response.h"
#include "priority_queue.h"
#include "buffer.h"
#include "http_parse.h"
#include "http_response.h"

#include <iostream>
using namespace std;

namespace CallBack
{          
    void request(HttpRequest* request, yj_priority_queue* queue)
    {
        queue->del_timer(request); //处理请求，删除定时器

        Buffer buf;
        int errno_ = 0;
        while(1) {
            int n = buf.readFd(request->getFd(), &errno_);
            if (n == 0) {
                std::cout << "n=0" << endl;
                //goto error;
            }
            if (n < 0 && (errno != EAGAIN)) {
                std::cout << "n<0 !EAGAIN" << endl;
                //goto error;
            }
            if (n < 0 && (errno == EAGAIN)) {
                std::cout << "n<0 == EAGAIN" << endl;
                break;
            }
        }

        HttpParse parse(request);
        parse.parseRequest(&buf);
        HttpResponse response(false);
        request->addHeaderFunc("Connection", std::bind(CallBack::handle_keep_alive,
            request, &response)); //添加头部字段回调函数
        request->handleFunc(); // 处理所有头部字段

        //判断文件存在

        //判断权限正确

        



        queue->add_timer(request, 2000, http_close_connect);//重置定时器,不断开连接
    }

    int http_close_connect(HttpRequest* request)
    {
       cout << "close fd: " << request->getFd() << endl;
        ::close(request->getFd());
        delete(request);  //close时释放request
        return 0;
    }

    void handle_keep_alive(HttpRequest* request, HttpResponse* response)
    {
        string s = request->getHeader("Connection");
        if (s == "keep-alive")
            response->setCloseConnection(false);
        if (s == "close")
            response->setCloseConnection(true);
    }
}