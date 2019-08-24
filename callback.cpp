#include "callback.h"
#include "http_request.h"
#include "http_response.h"
#include "priority_queue.h"
#include "buffer.h"
#include "http_parse.h"
#include "http_response.h"
#include "epoll.h"

       #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>
       #include <fcntl.h>
       #include <sys/mman.h>


#include <iostream>
using namespace std;

namespace CallBack
{          
    void request(HttpRequest* request, yj_priority_queue* queue, EPoll* poll)
    {
        queue->del_timer(request); //处理请求，删除定时器

        Buffer buf;
        int errno_ = 0;
        HttpParse parse(request);
        HttpResponse response(false);
        struct stat sbuf;
        string path("/home/yongjie/httpd");
        while(1) {
            int n = buf.readFd(request->getFd(), &errno_);
            if (n == 0) {
                std::cout << "n=0" << endl;
                goto error;
            }
            if (n < 0 && (errno != EAGAIN)) {
                std::cout << "n<0 !EAGAIN" << endl;
                goto error;
            }
            if (n < 0 && (errno == EAGAIN)) {
                std::cout << "n<0 == EAGAIN" << endl;
                break;
            }
        }

        parse.parseRequest(&buf);
        request->addHeaderFunc("Connection", std::bind(CallBack::handle_keep_alive,
            request, &response)); //添加头部字段回调函数
        request->handleFunc(); // 处理所有头部字段

        
        //判断文件存在
        path.append(request->getPath());
        if (stat(path.c_str(), &sbuf) < 0) {
            cout << "file doesn't exit" << endl;
            response.setStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
        } else if (!(S_ISREG((sbuf).st_mode)) || !(S_IRUSR & (sbuf).st_mode)) {
            //判断权限正确
            cout << "file can't read" << endl;
            response.setStatusCode(HttpResponse::HttpStatusCode::k403Fobidden);
        } else {
            //无错误
            cout << "open file success" << endl;
            response.setStatusCode(HttpResponse::HttpStatusCode::k200Ok);
            response.setContentType(string("text/html"));

            int src_fd = open(path.c_str(), O_RDONLY);
            char* src_addr = static_cast<char*>(mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0));
            response.setBody(string(src_addr, sbuf.st_size));
            close(src_fd);
            munmap(src_addr, sbuf.st_size);
            buf.retrieveAll();
            response.appendToBuffer(&buf);
            int count = buf.writeFd(request->getFd(), &errno_);
            cout << "write count: " << count << endl;
            if (count == sbuf.st_size)
                cout << "all size is send" << endl;
        }
        response.setStatusMessage(response.getMessageFromStatus());//设置message;

        if (response.closeConnection())
            goto error;

        queue->add_timer(request, 2000, http_close_connect);//重置定时器,不断开连接
        poll->epoll_mod(request->getFd(), request, (EPOLLIN | EPOLLET | EPOLLONESHOT));
        return;
    error:
        CallBack::http_close_connect(request);
    }

    int http_close_connect(HttpRequest* request)
    {
       cout << "close fd: " << request->getFd() << endl;
        ::close(request->getFd());
        //delete request;  //close时释放request
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