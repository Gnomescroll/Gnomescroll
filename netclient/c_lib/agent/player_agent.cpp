#pragma once

#include "player_agent.hpp"
#include <c_lib/agent/agent.hpp>

#ifdef DC_CLIENT

void PlayerAgent_state::set_PlayerAgent_id(int id) {
    agent_id = id;
}

void PlayerAgent_state::handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz) {
    state_snapshot.seq = seq;
    state_snapshot.theta = theta;
    state_snapshot.phi = phi;
    state_snapshot.x=x;state_snapshot.y=y;state_snapshot.z=z;
    state_snapshot.vx=vx;state_snapshot.vy=vy;state_snapshot.vz=vz;

    //do tick and update stuff
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

#endif

/*
void PlayerAgent_state::calculate_interpolate(int t) {
    float weight = (float) t / TICK_TIME;
    //interpolate.x = weight * (s.x - s_old.x) + s_old.x;
    //interpolate.y = weight * (s.y - s_old.y) + s_old.y;
    //interpolate.z = weight * (s.z - s_old.z) + s_old.z;
}
*/
