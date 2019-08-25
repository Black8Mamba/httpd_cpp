#include "priority_queue.h"
#include "timer.h"
#include <iostream>
#include <unistd.h>

using namespace std;

int main(void)
{

    yj_priority_queue queue;
    
    for (int i = 0; i < 10; ++i) {
        yj_timer* timer = new yj_timer();
        yj_timer::time_update();
        timer->set_key(yj_timer::current_msec);
        queue.insert(timer);
        sleep(1);
    }

    while(!queue.is_empty()) {
        cout << queue.min()->get_key() << endl;
        cout << queue.min() << endl;
        delete queue.min();
        queue.delmin();
    }

    return 0;
}