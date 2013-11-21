/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
   Licensed under GPLv3 */
#pragma once

#include <common/time/physics_timer.hpp>

void update_current_netpeer_time();
void netpeer_print_delta();

extern int LAST_NETPEER_TIME;
extern int CURRENT_NETPEER_TIME;

static inline int get_current_netpeer_time()
{
    return CURRENT_NETPEER_TIME;
}

static inline int netpeer_time_delta(int time)
{
    int delta = CURRENT_NETPEER_TIME - time;
    if (delta < 0)
    {
        printf("netpeer_time_delta: delta negative. rollover? delta=%i, current_netpeer_timer=%i, last_netpeer_time=%i \n", delta, CURRENT_NETPEER_TIME, LAST_NETPEER_TIME);
        return 0;
    }
    return delta;
}

static inline int netpeer_time_delta2(int t1, int t2)
{
    int delta = t2 - t1;
    if (delta < 0)
    {
        delta = (16777216-t1) + t2;
        printf("netpeer_time_delta2: delta negative. rollover? t1=%i,t2=%i,delta=%i \n", t1,t2,delta );
        return delta;
    }
    return delta;
}
