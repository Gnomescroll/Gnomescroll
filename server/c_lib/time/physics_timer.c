
#include "physics_timer.h"

int f;
long start_time;
long last_tick;
long tick_n;

long delta;

//long get_time() ;

long get_time() {
    #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        //ts.tv_sec = mts.tv_sec;
        //ts.tv_nsec = mts.tv_nsec;
        return mts.tv_sec*1000 + mts.tv_nsec/(1000*1000);
    #else
        #ifdef _POSIX_TIMERS
            struct timespec tp;
            //clock_gettime(CLOCK_REALTIME, &tp);
            clock_gettime(CLOCK_MONOTONIC, &tp);
            //printf("current time is: %ld \n", tp.tv_nsec);
            return tp.tv_sec*1000 + tp.tv_nsec/(1000*1000);
        #else
            printf("_POSIX_TIMERS not defined! \n");
        #endif
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
    if(delta < 0) {
        printf("physics_timer: delta less than zero!\n");
        delta =0;
        ///may cause timing bug
    }
    if(delta > 1000) {
        printf("physics_timer: delta is greater than 1 second!\n");
        delta = delta % f;
        return 1;
    }
    //printf("Delta= %ld \n", delta);
    if( delta >= f) {
        delta -= f;
        //printf("Tick!\n");
        tick_n++;
        last_tick = cur_t;
        return 1;
        //return delta/f; //number of ticks the server is behind
    } else {
        return 0;
    }
}
