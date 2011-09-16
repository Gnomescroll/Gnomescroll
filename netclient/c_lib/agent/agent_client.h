#ifndef agent_agent_client_h
#define agent_agent_client_h

#include <time/physics_timer.h>
#include <stdint.h>

#include <net_lib/common/type_pack.h>
#include <net_lib/client/client.h>

struct client_control_state {
    int seq;
    uint32_t cs;
    unsigned int tick;
    float theta;
    float phi;
};

struct client_agent_state {
    int id;

    int seq;
    struct client_control_state cs[128];

};


struct client_agent_state active_agent;

void init_agent_client();
void set_agent_control_state(int x[32], float theta, float phi);


#endif
