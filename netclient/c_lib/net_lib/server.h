#ifndef net_lib_client_h
#define net_lib_client_h

//#include <c_lib.h>

#define DC_SERVER

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <time/physics_timer.h>

//message handling
//#include <agent/control_state.h>

#include "server/server.h"

void _NetServerInit();
void _NetServerTick();


#endif
