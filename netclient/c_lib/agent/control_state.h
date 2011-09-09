#ifndef net_lib_agent_control_state_h
#define net_lib_agent_control_state_h

#include "../net_lib/common/type_pack.h"

#include "../net_lib/common/message_handler.h"


void init_agent_control_state();

#ifdef DC_CLIENT

void set_control_state(int* cs, float xangle, float yangle);
void PACK_control_state(unsigned char *buff, int* n);

#endif

#ifdef DC_SERVER

int handle_agent_control_state_message(unsigned char* buff, int n);

#endif

#endif
