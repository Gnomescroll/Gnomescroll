#pragma once

#include <common/time/physics_timer.hpp>

void update_current_netpeer_time();
void NP_print_delta();

extern int LAST_NETPEER_TIME;
extern int CURRENT_NETPEER_TIME;

static inline int get_current_netpeer_time() {
    return CURRENT_NETPEER_TIME;
}

static inline int NP_time_delta1(int time) {
    int delta = CURRENT_NETPEER_TIME - time;
    if ( delta<0) {
        
        printf("NP_time_delta1: delta negative. rollover? delta=%i, current_netpeer_timer=%i, last_netpeer_time=%i \n", delta, CURRENT_NETPEER_TIME, LAST_NETPEER_TIME);
        return 0;
    }
    return delta;
}

static inline int NP_time_delta2(int t1, int t2) {
    int delta = t2 - t1;
    if (delta < 0) {
        delta = (16777216-t1) + t2;
        printf("NP_time_delta2: delta negative. rollover? t1=%i,t2=%i,delta=%i \n", t1,t2,delta );
        return delta;
    }
    return delta;
}
