#include "physics_timer.hpp"

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

//#ifdef _WIN32
//    #include <Windows.h>
//#endif

int c_tick = 0;

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

int _last_tick;

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

		int t = cs_sec*1000/TICK_MS + cn_sec/(1000*1000)/TICK_MS;
        _ti = cs_sec*1000 + cn_sec/(1000*1000);
	#else
        static const long GetTickCount_start = GetTickCount();
		long cs_ms = GetTickCount() - GetTickCount_start;  
		int t = cs_ms / TICK_MS;		
        _ti = cs_ms;
	#endif
	
    if(c_tick < t) {
        if(c_tick+5 < t)
        {
            printf("Timer error: c_tick < t is %i < %i \n", c_tick,t);
            c_tick = t;
            return 1;
        }
        c_tick++;
        _last_tick = _ti;
        return 1;
    } else {

        return 0;
    }
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

int _LAST_TICK()
{
    return _last_tick;
}
//}