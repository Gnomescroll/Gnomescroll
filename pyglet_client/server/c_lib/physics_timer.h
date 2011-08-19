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
