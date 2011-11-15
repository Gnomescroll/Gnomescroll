#pragma once

#include "player_agent.hpp"
#include <c_lib/agent/agent.hpp>

#ifdef DC_CLIENT

void PlayerAgent_state::set_PlayerAgent_id(int id) {
    agent_id = id;
}

//set actually sends
void PlayerAgent_state::set_control_state(uint16_t cs, float theta, float phi) {
    if(agent_id == -1) return;  //player agent not set

    seq = (seq+1) % 256;

    Agent_cs_CtoS csp;

    csp.id = agent_id;
    csp.seq = seq;
    csp.cs = cs;
    csp.theta = theta;
    csp.phi = phi;

    csp.send();

    //printf("control state send: seq= %i, cs= %i \n", seq, cs);
}

static inline float _agent_weight(float t) {
    return pow(AGENT_INTERPOLATION_DECAY, t/TICK_DURATION);
}

// assumes constant time between history states, until delta_t is defined on the states
void PlayerAgent_state::calculate_interpolate() {
    interpolate.x = 0.0f;
    interpolate.y = 0.0f;
    interpolate.z = 0.0f;
    interpolate.vx = 0.0f;
    interpolate.vy = 0.0f;
    interpolate.vz = 0.0f;

    AgentState* a = &interpolate;
    float divisor, t, weight;
    divisor = t = 0.0f;
    
    int i, index;
    for (i=0; i<AGENT_STATE_HISTORY_SIZE; i++) {
        index = (AGENT_STATE_HISTORY_SIZE + state_history_index - i) % AGENT_STATE_HISTORY_SIZE;

        weight = _agent_weight(t);
        divisor += weight;

        a->x += state_history[index].x;
        a->y += state_history[index].y;
        a->z += state_history[index].z;
        a->vx += state_history[index].vx;
        a->vy += state_history[index].vy;
        a->vz += state_history[index].vz;

        t += TICK_DURATION;
        //t += state_history[index].dt;
    }

    a->x /= divisor;
    a->y /= divisor;
    a->z /= divisor;
    a->vx /= divisor;
    a->vy /= divisor;
    a->vz /= divisor;
}

#endif
