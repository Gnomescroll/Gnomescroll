#include "physics_timer.hpp"

#include <stdio.h>
#include <time.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#ifdef __MSVC__
#include <Windows.h>
#include <WinBase.h>
#endif

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

//#ifdef _WIN32
//    #include <Windows.h>
//#endif

long c_tick = 0;

void _START_CLOCK() {
/*
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
            //printf("_POSIX_TIMERS not defined! \n");
            window_ms_start = GetTickCount();
        #endif
    #endif
*/
}

#define TICK_MS 33.3333

long _last_tick;

int _GET_TICK() {
    //long s_sec, n_sec;
    long _ti;
    #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        long s_sec, n_sec;

        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        s_sec = mts.tv_sec;
        n_sec = mts.tv_nsec;

        static const long s_sec_start = mts.tv_sec;
        static const long n_sec_start = mts.tv_nsec;

    #else
        #ifdef _POSIX_TIMERS
            
            long s_sec, n_sec;

            struct timespec tp;
            //clock_gettime(CLOCK_REALTIME, &tp);
            clock_gettime(CLOCK_MONOTONIC, &tp);
            s_sec = tp.tv_sec;
            n_sec = tp.tv_nsec;

            static const long s_sec_start = s_sec;
            static const long n_sec_start = n_sec;
        #else
            //printf("_POSIX_TIMERS not defined! \n");
        #endif
    #endif
    
    #ifdef _POSIX_TIMERS
        long cs_sec, cn_sec;
        cs_sec = s_sec - s_sec_start;
        cn_sec = n_sec - n_sec_start;

        long t = (long)(cs_sec*1000/TICK_MS + cn_sec/(1000*1000)/TICK_MS);
        _ti = cs_sec*1000 + cn_sec/(1000*1000);
    #else
        static const long GetTickCount_start = GetTickCount();
        long cs_ms = GetTickCount() - GetTickCount_start;  
        long t = cs_ms / TICK_MS;       
        _ti = cs_ms;
    #endif
    
    if(c_tick < t) {
        if(c_tick+5 < t)
        {
            #if DC_SERVER || !PRODUCTION
            printf("Timer error: c_tick < t is %ld < %ld \n", c_tick,t);
            #endif
            c_tick = t;
            return 1;
        }
        c_tick++;
        _last_tick = _ti;
        return 1;
    } else 
    {
        return 0;
    }

    return 0;
}

int _GET_MS_TIME() 
{
    //long s_sec, n_sec;
    #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        long s_sec, n_sec;   
        
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        s_sec = mts.tv_sec;
        n_sec = mts.tv_nsec;
        static const long s_sec_start = mts.tv_sec;
        static const long n_sec_start = mts.tv_nsec;
    #else
        #ifdef _POSIX_TIMERS

            long s_sec, n_sec;

            struct timespec tp;
            //clock_gettime(CLOCK_REALTIME, &tp);
            clock_gettime(CLOCK_MONOTONIC, &tp);
            s_sec = tp.tv_sec;
            n_sec = tp.tv_nsec;
            static const long s_sec_start = s_sec;
            static const long n_sec_start = n_sec;
        #else
            //printf("_POSIX_TIMERS not defined! \n");
        #endif
    #endif
    
    #ifdef _POSIX_TIMERS
        long cs_sec, cn_sec;
        cs_sec = s_sec - s_sec_start;
        cn_sec = n_sec - n_sec_start;
        return (int) (cs_sec*1000+ (cn_sec/ 1000000) );
    #else
        static const long GetTickCount_start = GetTickCount();
        return GetTickCount() - GetTickCount_start;  
    #endif
}

long _GET_MICROSECOND_TIME() 
{
    //long s_sec, n_sec;
    #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        long s_sec, n_sec;

        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        s_sec = mts.tv_sec;
        n_sec = mts.tv_nsec;

        static const long s_sec_start = mts.tv_sec;
        static const long n_sec_start = mts.tv_nsec;

    #else
        #ifdef _POSIX_TIMERS
            long s_sec, n_sec;

            struct timespec tp;
            //clock_gettime(CLOCK_REALTIME, &tp);
            clock_gettime(CLOCK_MONOTONIC, &tp);
            s_sec = tp.tv_sec;
            n_sec = tp.tv_nsec;

            static const long s_sec_start = s_sec;
            static const long n_sec_start = n_sec;

        #else
            //printf("_POSIX_TIMERS not defined! \n");
        #endif
    #endif
    
    #ifdef _POSIX_TIMERS
        long cs_sec, cn_sec;
        cs_sec = s_sec - s_sec_start;
        cn_sec = n_sec - n_sec_start;
        return cs_sec*(1000*1000) + (cn_sec/ 1000);
    #else
        static const long GetTickCount_start = GetTickCount();
        return 1000*(GetTickCount() - GetTickCount_start);  
    #endif
}


void _SET_LAST_TICK()
{
    _last_tick = _GET_MS_TIME();
}

long _LAST_TICK()
{
    return _last_tick;
}

void gs_millisleep(int milliseconds)
{   // NOTE: milliseconds must be <1000
    GS_ASSERT(milliseconds > 0 && milliseconds < 1000);
    #ifdef _WIN32
    Sleep(milliseconds);
    #else
    struct timespec req;
    req.tv_sec = 0; // 0 seconds
    req.tv_nsec = milliseconds*1000*1000;   // nanoseconds
    nanosleep(&req, NULL);
    #endif 
}

void gs_microsleep(int microseconds)
{
    #ifdef _WIN32
    Sleep((float)microseconds/1000.0f);
    #else
    struct timespec req;
    req.tv_sec = 0; // 0 seconds
    req.tv_nsec = microseconds*1000;   // nanoseconds
    nanosleep(&req, NULL);
    #endif 
}
