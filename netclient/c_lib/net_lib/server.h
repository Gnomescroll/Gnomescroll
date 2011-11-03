#pragma once

#ifdef DC_SERVER

//#include <c_lib.h>

//#define DC_SERVER

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <time/physics_timer.h>
#include <net_lib/server/server.h>

//message handling
//#include <agent/control_state.h>

//extern "C" {
    void _NetServerInit();
    void _NetServerTick();
//}

#endif
