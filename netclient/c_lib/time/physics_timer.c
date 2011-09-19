

#include "physics_timer.h"


//extern "C" {

int f;
long start_time;
long last_tick;
long tick_n;

long delta;

//long Cget_time() ;

long Cget_time() {
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
    return Cget_time();
}

long _get_tick() {
    return tick_n;
}

int _start_physics_timer(int frequency) {
    printf("Physics timer started: one tick is %i ms \n", frequency);
    tick_n = 0;
    delta = 0;
    f = frequency;
    start_time = Cget_time();
    last_tick = start_time;
    return 0;
}

int _tick_check() {
    long cur_t = Cget_time();

    //printf("Time since last tick= %ld \n", cur_t - last_tick);
    delta += cur_t - last_tick;
    if(delta < 0) {
        printf("physics_timer: delta less than zero!\n");
        delta =0;
        ///may cause timing bug
    }
    if(delta > 1000) {
        printf("physics_timer: delta is greater than 1 seconds!\n");
        delta = 0;
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

int start_sec;
int start_nsec;
int c_tick = 0;

void _START_CLOCK() {
    c_tick = 0;
    #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        start_sec  = mts.tv_sec;
        start_nsec = mts.tv_nsec;
    #else
        #ifdef _POSIX_TIMERS
            struct timespec tp;
            //clock_gettime(CLOCK_REALTIME, &tp);
            clock_gettime(CLOCK_MONOTONIC, &tp);
            start_sec = tp.tv_sec;
            start_nsec = tp.tv_nsec;
        #else
            printf("_POSIX_TIMERS not defined! \n");
        #endif
    #endif
}

#define TICK_MS 33

int _GET_TICK() {
    int s_sec, n_sec;
    #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        s_sec = mts.tv_sec;
        n_sec = mts.tv_nsec;
    #else
        #ifdef _POSIX_TIMERS
            struct timespec tp;
            //clock_gettime(CLOCK_REALTIME, &tp);
            clock_gettime(CLOCK_MONOTONIC, &tp);
            s_sec = tp.tv_sec;
            n_sec = tp.tv_nsec;
        #else
            printf("_POSIX_TIMERS not defined! \n");
        #endif
    #endif
    int cs_sec, cn_sec;
    cs_sec = s_sec - start_sec;
    cn_sec = n_sec - start_nsec;

    //printf("s= %i n=%i \n", cs_sec, cn_sec/1000000);

    int t = cs_sec*1000/TICK_MS + cn_sec/(1000*1000)/TICK_MS;
    //printf("t=%i ;%i, %i \n", t, cs_sec*1000/TICK_MS, cn_sec/(1000*1000)/TICK_MS);
    //printf("d=%i t=%i c=%i \n", t - c_tick, t, c_tick);
    if(c_tick < t) {
        c_tick++;
        return 1;
        //return t-c_tick;
        //return 1;
    } else {
        return 0;
    }
}

int _GET_MS_TIME() {
    int s_sec, n_sec;
    #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        s_sec = mts.tv_sec;
        n_sec = mts.tv_nsec;
    #else
        #ifdef _POSIX_TIMERS
            struct timespec tp;
            //clock_gettime(CLOCK_REALTIME, &tp);
            clock_gettime(CLOCK_MONOTONIC, &tp);
            s_sec = tp.tv_sec;
            n_sec = tp.tv_nsec;
        #else
            printf("_POSIX_TIMERS not defined! \n");
        #endif
    #endif
    int cs_sec, cn_sec;
    cs_sec = s_sec - start_sec;
    cn_sec = n_sec - start_nsec;
    //printf("ms_time= %i\n",cs_sec*1000+ (cn_sec/ 1000000));
    return cs_sec*1000+ (cn_sec/ 1000000);

}

//}