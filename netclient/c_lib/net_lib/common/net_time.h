#ifndef net_lib_common_net_time_h
#define net_lib_common_net_time_h

#include "net_lib_common.h"


#include <time/physics_timer.h>

void update_current_netpeer_time();
void NP_print_delta();
//int NP_time_delta1(int time);
//int NP_time_delta2(int time1, int time2);

//int _GET_MS_TIME();

extern int LAST_NETPEER_TIME;
extern int CURRENT_NETPEER_TIME;

static inline int get_current_netpeer_time() {
    return CURRENT_NETPEER_TIME;
}

static inline int NP_time_delta1(int time) {
    int delta = CURRENT_NETPEER_TIME - time;
    if( delta<0) {
        printf("NP_time_delta1: delta negative. rollover? \n");
        return 0;
    }
    return delta;
}

static inline int NP_time_delta2(int t1, int t2) {
    int delta = t2 - t1;
    if(delta < 0) {
        delta = (16777216-t1) + t2;
        printf("NP_time_delta2: delta negative. rollover? t1=%i,t2=%i,delta=%i \n", t1,t2,delta );
        return delta;
    }
    return delta;
}


#endif
