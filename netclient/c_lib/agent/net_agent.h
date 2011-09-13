#ifndef net_agent_c
#define net_agent_c

#include <stdlib.h>
#include <stdint.h>
#include "../netlib/common/type_pack.h"

#include "./agent2.h"


void client_init_net_agent();
void server_init_net_agent();

void handle_NET_agent_control_state(unsigned char* buff, int n);
void handle_NET_agent_snapshot(unsigned char* buff, int n);

#endif
