#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>

using namespace std;

int main(void)
{
	int event_fd;
	event_fd = epoll_create(1024);
	assert(event_fd > 0);
	int fd = open("./doc", O_RDWR);
	assert(fd > 0);
	struct epoll_event event;
	event.events = (EPOLLIN | EPOLLET);
	event.data.fd = fd;
	int ret = epoll_ctl(event_fd, EPOLL_CTL_ADD, fd, &event);
	//assert(ret == 0);
	perror("failed\n");
	cout << "ret: " << ret << endl;

	switch(fork()) {
		case 0 : //child
			sleep(5);
			close(fd);
			break;
		default:  //parent
			struct epoll_event event;
			int num = epoll_wait(event_fd, &event, 1024, -1);
			cout << num << endl;
			if (event.events & EPOLLIN)
				cout << "EPOLLIN" << endl;
			if (event.events & EPOLLHUP)
				cout << "EPOLLHUP" << endl;
			break;

	}

	return 0;
}