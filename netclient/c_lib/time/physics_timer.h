#pragma once

#include <stdio.h>
//#include <stdlib.h>

#include <time.h>

#include <unistd.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

//extern "C" {
    int _start_physics_timer(int frequency); //ms per frame

    int _tick_check();

    long _get_time();
    long _get_tick();

    void _START_CLOCK();
    int _GET_TICK();
    int _GET_MS_TIME();

    float _TIME_DELTA();
//}
