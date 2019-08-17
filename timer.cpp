#include <sys/time.h>
#include "timer.h"

size_t yj_timer::current_msec = 0; 

//更新当前时间
void yj_timer::time_update()
{
    struct timeval tv;
    int rc = gettimeofday(&tv, NULL); //设置全局变量erron？ return -1？
    yj_timer::current_msec = ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

