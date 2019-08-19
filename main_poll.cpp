#include "epoll.h"
#include <unistd.h>
#include <fcntl.h> 
#include <iostream>
#include <sys/types.h>

using namespace std;

int main(void)
{
    EPoll poll(0);
    int filedes[2];
    pipe2(filedes, O_NONBLOCK);
                char buf[10] = {0};
    http_request request(2);
    switch(fork()) {
        case -1:
            return -1;
            break;
        case 0:
            sleep(3);

            write(filedes[1], buf, sizeof(buf));
            break;
        default:
         poll.epoll_add(filedes[0], &request, (EPOLLIN | EPOLLET));
         poll.epoll_wait(1024, 5000);
         std::cout << poll.get_epoll_num_events() << endl;
         EPoll::EventList events = poll.get_epoll_events();
         http_request* re = static_cast<http_request*>(events[0].data.ptr);
         std::cout << re->num_ << endl;
         break;
    }

    return 0;
}