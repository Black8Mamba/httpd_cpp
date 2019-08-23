#include "socket.h"
#include "inet_address.h"
#include "socket_ops.h"
#include <iostream>
#include "epoll.h"
#include "threadpool.h"
#include "current_thread.h"
#include "count_down_latch.h"
#include <functional>
#include <errno.h>
#include "buffer.h"
#include <string.h>
#include "http_parse.h"
#include "priority_queue.h"
#include "http_response.h"

       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <sys/mman.h>



using namespace std;

void print(int fd)
{
    printf("tid=%d, fd: %d\n", CurrentThread::tid(), fd);
}

int http_close_connect(HttpRequest* request)
{
    cout << "close fd: " << request->getFd() << endl;
    ::close(request->getFd());
    delete(request);  //close时释放request
    //std::cout << "close fd: " << request->getFd() << endl;
    return 0;
}

int main()
{
    ThreadPool pool("MainThreadPool");
    Buffer buf;
    pool.setMaxQueueSize(7);
    pool.start(4);
    EPoll poll(0);
    HttpRequest request;
    InetAddress addr(3001);
   // InetAddress addr_client(0);
    int sockfd = sockets::create_nonblock_socket(AF_INET);
    request.setFd(sockfd);
    cout << "sockfd:" << sockfd << endl;
    Socket socket(sockfd);
    socket.setReuseAddr(true);
    socket.bindAddress(addr);
    socket.listen();
    //int connfd = socket.accept(&addr_client);
    poll.epoll_add(socket.fd(), &request, (EPOLLIN | EPOLLET));
    yj_priority_queue queue;

    while(1) {
        poll.epoll_wait(1024, -1);
        int num = poll.get_epoll_num_events();
        std::cout << "num_events:" << num << endl;
        if (num == 0)
            continue;
        queue.handle_expire_timers();  //处理超时的请求
        for (int i = 0; i < num; ++i) {
            //InetAddress addr_client(0);
            //int connfd = socket.accept(&addr_client);
            //cout << "connfd:" << connfd << endl;
            //pool.run(std::bind(print, connfd));
            HttpRequest* request = static_cast<HttpRequest*>(poll.get_epoll_events()[i].data.ptr);
            if (request->getFd() == sockfd) {
                InetAddress addr_client(0);
                int connfd = socket.accept(&addr_client);
                cout << "connfd connect:" << connfd << endl;
                HttpRequest *re = new HttpRequest();  //接受连接时申请request；
                re->setFd(connfd);
                queue.add_timer(re, 2000, http_close_connect);
                poll.epoll_add(connfd, re, (EPOLLIN | EPOLLET | EPOLLONESHOT));
            } else {
                queue.del_timer(request); //处理请求，删除定时器
                cout << "connfd read:" << request->getFd() << endl;
                if ((poll.get_epoll_events()[i].events & EPOLLERR) || 
                    (poll.get_epoll_events()[i].events & EPOLLHUP)  || 
                    (!(poll.get_epoll_events()[i].events & EPOLLIN))) {
                        std::cout << "etner !POLLIN" << endl;
                        close(request->getFd());
                        poll.epoll_del(request->getFd(), request, (EPOLLIN | EPOLLET | EPOLLONESHOT));
                        continue;
                    }
                Buffer buf;
                int errno_ = 0;
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
                
                strerror(errno_);
                //cout << buf.retrieveAllAsString() << endl;
                HttpParse parse(request);
                parse.parseRequest(&buf);
                HttpResponse response(false);
                response.setContentType("text/html");
                response.setStatusCode(HttpResponse::HttpStatusCode::k200Ok);
                response.setStatusMessage(response.getMessageFromStatus());
                int src_fd = open("./index.html", O_RDONLY);
                char *src_addr = static_cast<char*>(mmap(NULL, 538, PROT_READ, MAP_PRIVATE, src_fd, 0));
                string s(src_addr, 538);
                response.setBody(s);
                buf.retrieveAll();
                response.appendToBuffer(&buf);
                int n = buf.writeFd(request->getFd(), &errno_);
                cout << "write byte: " << n << endl;
               queue.add_timer(request, 2000, http_close_connect);//重置定时器,不断开连接
            }
            
            continue;
            error:
                close(request->getFd());
        }
    }

 return 0;
}