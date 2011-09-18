#ifndef net_lib_client_h
#define net_lib_client_h

#include <c_lib.h>

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <net_lib/client/client.hpp>

#include <time/physics_timer.h>

//message handling
//#include <agent/control_state.h>

//#include <agent/agent_client.h>

void _NetClientConnect(int a, int b,int c, int d, int _port);
void _NetClientTick();

#endif
