#ifndef net_lib_client_h
#define net_lib_client_h

#include <c_lib.h>

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "client/client.h"

#include <time/physics_timer.h>

//message handling
#include <agent/control_state.h>

void _NetClientConnect(int a, int b,int c, int d, int _port);
void _NetClientTick();

#endif
