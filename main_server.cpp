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
#include "callback.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>



using namespace std;


int main()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigaction(SIGPIPE, &sa, NULL); //忽略SIGPIPE信号
        

    ThreadPool pool("MainThreadPool");
    pool.setMaxQueueSize(20); //任务队列大小
    pool.start(2);
    EPoll poll(0);
    HttpRequest request;
    InetAddress addr(3001);
    int sockfd = sockets::create_nonblock_socket(AF_INET);
    request.setFd(sockfd);
    cout << "sockfd:" << sockfd << endl;
    Socket socket(sockfd);
    socket.setReuseAddr(true);
    socket.bindAddress(addr);
    socket.listen();
    poll.epoll_add(socket.fd(), &request, (EPOLLIN | EPOLLET));
    yj_priority_queue queue;  //定时器的优先级队列

    while(1) {
        poll.epoll_wait(1024, -1);
        int num = poll.get_epoll_num_events();
        std::cout << "num_events:" << num << endl;
        if (num == 0)
            continue;
        queue.handle_expire_timers();  //处理超时的请求,超时了关闭fd，下面又进行处理？
        for (int i = 0; i < num; ++i) {
            HttpRequest* request = static_cast<HttpRequest*>(poll.get_epoll_events()[i].data.ptr);
            if (request->getFd() == sockfd) {
                InetAddress addr_client(0);
                int connfd = socket.accept(&addr_client); //可能有多个客户端accept,如何处理，while
                cout << "connfd connect:" << connfd << endl;
                HttpRequest *re = new HttpRequest();  //接受连接时申请request；什么时候释放？close的时候，如何保证close一次
                re->setFd(connfd);
                queue.add_timer(re, 2000, CallBack::http_close_connect);//加入超时队列
                poll.epoll_add(connfd, re, (EPOLLIN | EPOLLET | EPOLLONESHOT));//add epoll
            } else {
                cout << "connfd read:" << request->getFd() << endl;
                   if (poll.get_epoll_events()[i].events & EPOLLIN) {
                        cout << "Pollin" << endl;
                       pool.run(bind(CallBack::request, request, &queue, &poll)); //queue poll 线程安全？
                       //CallBack::request(request, &queue, &poll);
                   }else { //错误如何处理
                        if (poll.get_epoll_events()[i].events & EPOLLPRI)
                            cout << "EPOLLPRI" << endl;
                        else if (poll.get_epoll_events()[i].events & EPOLLRDHUP)
                            cout << "EPOLLRDHUP" << endl;
                        else if (poll.get_epoll_events()[i].events & EPOLLOUT)
                            cout << "EPOLLOUT" << endl;
                        else if (poll.get_epoll_events()[i].events & EPOLLERR)
                            cout << "EPOLLERR" << endl;
                        else if (poll.get_epoll_events()[i].events & EPOLLHUP)
                            cout << "EPOLLHUP" << endl;
                        else
                            cout << "UNKONOW" << endl;
                        //移出定时器队列，close fd, delete request;
                        queue.del_timer(request);
                        ::close(request->getFd());
                        poll.epoll_del(request->getFd(), request, (EPOLLIN | EPOLLET | EPOLLONESHOT));
                        delete request;
                   }
            }
        }
    }

 return 0;
}