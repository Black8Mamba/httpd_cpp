#include "socket.h"
#include "inet_address.h"
#include "socket_ops.h"
#include <iostream>
#include "epoll.h"

using namespace std;

int main()
{
    EPoll poll(0);
    http_request request(2);
    InetAddress addr(6669);
    InetAddress addr_client(0);
    int sockfd = sockets::create_nonblock_socket(AF_INET);
    cout << "sockfd:" << sockfd << endl;
    Socket socket(sockfd);
    socket.setReuseAddr(true);
    socket.bindAddress(addr);
    socket.listen();
    //int connfd = socket.accept(&addr_client);
    poll.epoll_add(socket.fd(), &request, (EPOLLIN | EPOLLET));

    poll.epoll_wait(1024, 8000);
    std::cout << poll.get_epoll_num_events() << endl;
    int connfd = socket.accept(&addr_client);

    char buf[] = "hello\n";
    cout << "connfd:" << connfd << endl;
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




//  InetAddress addr_client(0);
 //   int connfd = socket.accept(&addr_client);
 //   cout << connfd << endl;

 return 0;
}