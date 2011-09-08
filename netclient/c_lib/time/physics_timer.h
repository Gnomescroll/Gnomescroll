
#ifndef time_physics_timer_h
#define time_physics_timer_h

#include <stdio.h>
//#include <stdlib.h>

#include <time.h>

#include <unistd.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

int _start(int frequency); //ms per frame

int _tick_check();

long _get_time();
long _get_tick();

void _START_CLOCK();
int _GET_TICK();

#endif
