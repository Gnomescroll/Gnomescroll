
#include "physics_timer.h"

int f;
long start_time;
long last_tick;
long tick_n;

struct timespec s_time;
struct timespec s_last_tick;

//long get_time() ;

long get_time() {
    #ifdef _POSIX_TIMERS
        struct timespec tp;
        //clock_gettime(CLOCK_REALTIME, &tp);
        clock_gettime(CLOCK_MONOTONIC, &tp);

        //printf("current time is: %ld \n", tp.tv_nsec);
        return tp.tv_sec*1000 + tp.tv_nsec/(1000*1000);
    #else
        printf("_POSIX_TIMERS not defined! \n");
    #endif
}

long _get_time() {
    return get_time();
}

int _start(int frequency) {
    printf("Physics timer started: one tick is %ld ms \n", frequency);
    tick_n = 0;
    f = frequency;
    start_time = get_time();
    last_tick = start_time;
}

int _tick_check() {
    long cur_t = get_time();

    printf("Time since last tick= %ld \n", cur_t - last_tick);
    if( cur_t - last_tick >= f) {
        printf("Tick!\n");
        tick_n++;
        last_tick = cur_t;
        return 1;
    } else {
        return 0;
    }
}
