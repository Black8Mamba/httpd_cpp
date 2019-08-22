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

using namespace std;

void print(int fd)
{
    printf("tid=%d, fd: %d\n", CurrentThread::tid(), fd);
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

    while(1) {
        poll.epoll_wait(1024, -1);
        int num = poll.get_epoll_num_events();
        std::cout << "num_events:" << num << endl;
        if (num == 0)
            continue;
        
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
                HttpRequest *re = new HttpRequest();
                re->setFd(connfd);
                poll.epoll_add(connfd, re, (EPOLLIN | EPOLLET | EPOLLONESHOT));
            } else {
                cout << "connfd read:" << request->getFd() << endl;
                
                if ((poll.get_epoll_events()[i].events & EPOLLERR) || 
                    (poll.get_epoll_events()[i].events & EPOLLHUP)  || 
                    (!(poll.get_epoll_events()[i].events & EPOLLIN))) {
                        close(request->getFd());
                        continue;
                    }
                Buffer buf;
                int errno_ = 0;
                buf.readFd(request->getFd(), &errno_);
                strerror(errno_);
                //cout << buf.retrieveAllAsString() << endl;
                HttpParse parse(*request);
                parse.parseRequest(&buf);
                cout << parse.getRequest().methodString() << endl;
                cout << parse.getRequest().getVersion() << endl;
                cout << parse.getRequest().getPath() << endl;
                parse.getRequest().printHeaders();
                char message[] = "HTTP/1.1 404 Not Found\r\n";
                //::write(request->getFd(), message, sizeof(message));
               // cout <<  parse.getRequest().getHeader("Host") << endl;
               // cout << parse.getRequest().getHeader("Connection") << endl;
               
            }

            
        }
       /*
       int connfd;
       InetAddress addr_client(0);
       while((connfd = socket.accept(&addr_client)) != -1) {
           cout << "connfd:" << connfd << endl;
           pool.run(std::bind(print, connfd));
       }
       if (errno == EAGAIN )
            cout << "errno:EAGAIN " << endl;
        */
            
       // poll.epoll_add(socket.fd(), &request, (EPOLLIN | EPOLLET));
    }


    //char buf[] = "hello\n";

    
    
    /*
    sockets::write(connfd, buf, sizeof(buf));

    poll.epoll_add(connfd, &request,  (EPOLLIN | EPOLLET));
    while(1) {
        poll.epoll_wait(1024, 8000);
        std::cout << poll.get_epoll_num_events() << endl;
        char buf2[1024] = {0};
        while(errno != EAGAIN)
            sockets::read(connfd, buf2, sizeof(buf));
        printf("%s", buf2);
        sockets::write(connfd, buf, sizeof(buf));
    }
    */




//  InetAddress addr_client(0);
 //   int connfd = socket.accept(&addr_client);
 //   cout << connfd << endl;

 return 0;
}