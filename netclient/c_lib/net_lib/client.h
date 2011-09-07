#ifndef net_lib_client_h
#define net_lib_client_h

#include "client/client.h"
#include "common/net_peer.h"
#include "common/net_packets.h"
#include "common/sequencer.h"

void _NetClientConnect(int a, int b,int c, int d, int _port);
void _NetClientTick();

#endif
