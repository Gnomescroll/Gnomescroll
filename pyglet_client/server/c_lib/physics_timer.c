
#include "physics_timer.h"

int f;
long start_time;
long last_tick;
long tick_n;

long delta;

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

long _get_tick() {
    return tick_n;
}

int _start(int frequency) {
    printf("Physics timer started: one tick is %i ms \n", frequency);
    tick_n = 0;
    delta = 0;
    f = frequency;
    start_time = get_time();
    last_tick = start_time;
}

int _tick_check() {
    long cur_t = get_time();

    //printf("Time since last tick= %ld \n", cur_t - last_tick);
    delta += cur_t - last_tick;
    printf("Delta= %ld \n", delta);
    if( delta >= f) {
        delta -= f;
        printf("Tick!\n");
        tick_n++;
        last_tick = cur_t;
        return 1;
    } else {
        return 0;
    }
}
