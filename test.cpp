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

int main(void)
{

	Buffer buf;
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

	while(1) {
		poll.epoll_wait(1024, -1);
		int num = poll.get_epoll_num_events();
		std::cout << "num_events:" << num << endl;
		if (num == 0)
            continue;
        for (int i = 0; i < num; ++i) {
        	HttpRequest* request = static_cast<HttpRequest*>(poll.get_epoll_events()[i].data.ptr);
        	if (request->getFd() == sockfd) {
        		InetAddress addr_client(0);
        		while(1) {
        			int connfd = socket.accept(&addr_client);
        			if (connfd <= 0)
        				break;
        			else {
        				HttpRequest *re = new HttpRequest();
     					cout << "connect fd:" << connfd << endl;
     					poll.epoll_add(connfd, re, (EPOLLIN | EPOLLET));  					
        			}
        		}
        	} else {
        		   		std::cout  << "read fd" << request->getFd() << endl;
        				int errno_ = 0;
        				int n = buf.readFd(request->getFd(), &errno_);
        				if (n == 0) {
        					cout << "n==0" << endl;
        				} else
        					cout << buf.retrieveAllAsString() << endl;
        	}
        }
	}

	return 0;
}