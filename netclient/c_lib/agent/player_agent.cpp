#pragma once

#include "player_agent.hpp"
#include <c_lib/agent/agent.hpp>

//#ifdef DC_CLIENT

void PlayerAgent_state::set_PlayerAgent_id(int id) {
    agent_id = id;
}

void PlayerAgent_state::handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz) {
    //printf("should never be called");


    class AgentState ss;

    ss.seq = seq;
    ss.theta = theta;
    ss.phi = phi;
    ss.x=x;ss.y=y;ss.z=z;
    ss.vx=vx;ss.vy=vy;ss.vz=vz;

    int index = seq%8;

    state_history[index] = ss;


    if((state_history_index+1)%8 == index) state_history_index = index;

    //do tick and update stuff

    //printf("received snaphshot %i\n", seq);
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

}

static inline float _agent_weight(float t) {
    return pow(AGENT_INTERPOLATION_DECAY, t/TICK_DURATION);
}


// assumes constant time between history states, until delta_t is defined on the states
void PlayerAgent_state::calculate_interpolate() {
/*
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

*/
}

