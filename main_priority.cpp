#include "timer.h"
#include <iostream>
#include "priority_queue.h"
#include <unistd.h>


using namespace std;
/* 
int main()
{
	yj_timer timer1, timer2, timer3, timer4;
	timer1.set_key(123);
	timer2.set_key(91);
	timer3.set_key(43);
	timer4.set_key(522);

	yj_priority_queue pq;
	cout << "empty: " << pq.is_empty() << endl;
	pq.insert(&timer1);
	pq.insert(&timer2);
	pq.insert(&timer3);
	pq.insert(&timer4);
	cout << "size: " << pq.size() << endl;
        for (int i= 0; i < 4; ++i) {
		cout << pq.min()->get_key() << endl;
		pq.delmin();
	}	


	return 0;
}
*/

int timeout_(http_request* request)
{
    std::cout << request->num_ << std::endl;
    return 0; 
}

int main(void)
{
	http_request r1(1);
	http_request r2(2);
	http_request r3(3);
	http_request r4(4);

	yj_priority_queue q;
	q.add_timer(&r1, 1000, timeout_);
	sleep(1);
	q.handle_expire_timers();
	q.add_timer(&r2, 1000, timeout_);
	sleep(1);
	q.handle_expire_timers();
	q.add_timer(&r3, 1000, timeout_);
	sleep(1);
	q.handle_expire_timers();
	q.add_timer(&r4, 1000, timeout_);
	sleep(1);
	q.handle_expire_timers();

	return 0;
}
