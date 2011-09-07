#ifndef net_lib_client_h
#define net_lib_client_h

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "client/client.h"

void _NetClientConnect(int a, int b,int c, int d, int _port);
void _NetClientTick();

#endif
