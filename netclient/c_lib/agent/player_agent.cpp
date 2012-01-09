#pragma once

#include "player_agent.hpp"

#include <c_lib/agent/agent.hpp>
#include <c_lib/ray_trace/ray_trace.h>

#ifdef DC_CLIENT
#include <c_lib/sound/sound.hpp>
#endif

#include <c_lib/time/physics_timer.h>
#include <c_lib/state/client_state.hpp>

/*
Net Messages
*/

//#include <net_lib/net.hpp>


/*
Other Stuff
*/

void PlayerAgent_state::set_PlayerAgent_id(int id) {
    agent_id = id;
}


void PlayerAgent_state::update_client_side_prediction_interpolated()
{
    int last_tick = _LAST_TICK();
    int _t = _GET_MS_TIME();

    //printf("ms since last update= %i \n", _t - last_tick);
    float delta = ((float)(_t - last_tick)) / 33.0f;

    if(delta > 1.0f)
    {
        //printf("PlayerAgent_state::update_client_side_prediction_interpolated: delta >1\n");
        delta = 1.0f;
    }


    if(delta < 0.0f)
    {
        printf("PlayerAgent_state::update_client_side_prediction_interpolated: delta is negative\n");
    }

    c.x = s0.x*(1-delta) + s1.x*delta;
    c.y = s0.y*(1-delta) + s1.y*delta;
    c.z = s0.z*(1-delta) + s1.z*delta;  

#ifdef DC_CLIENT // whole file should be ifdef'd?
    Agent_state* A = ClientState::agent_list.get(agent_id);
    if (A==NULL) return;
    c.theta = A->s.theta;
    c.phi = A->s.phi;
#endif

/*
    c.x = s1.x*(1-delta) + s0.x*delta;
    c.y = s1.y*(1-delta) + s0.y*delta;
    c.z = s1.z*(1-delta) + s0.z*delta;  
*/
}

void PlayerAgent_state::handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz) {
    //printf("should never be called");

    //printf("snapshot cs= %i, z= %f \n", seq, z);
    //printf("snapshot cs= %i \n", seq );

    class AgentState ss;

    ss.seq = seq;
    ss.theta = theta;
    ss.phi = phi;
    ss.x=x;ss.y=y;ss.z=z;
    ss.vx=vx;ss.vy=vy;ss.vz=vz;

    int index = seq%AGENT_STATE_HISTORY_SIZE;

    state_history[index] = ss;

    //if(! ((state_history_index+1)%AGENT_STATE_HISTORY_SIZE == index) ) 
    //{
    //    printf("PlayerAgent_state::handle_state_snapshot: ERROR out of sequence agent state snapshot!! WTF!?!?! \n");
    //}

    //snapshot_net[seq%128] = ss; //snapshot net is for detecting prediction errors

    if( (state_history_seq - seq) > 30 || seq > state_history_seq) {

        state_history_index = index;
        state_history_seq = seq; //set index
        
        state_snapshot = ss;    //set state snapsot
    }

/*
    int i;
    for(i=48;i<64;i++){
        index = (seq + i)%128;
        state_history[index].seq = -1;
    }
*/

}

#ifdef DC_CLIENT
//take outgoing control input and do client side prediction
//seq for prediction will always exceed client side one
void PlayerAgent_state::handle_local_control_state(int _seq, int _cs, float _theta, float _phi) {
    //printf("control state received: agent=%i, seq=%i, cs=%i \n", id, _seq, _cs);
    
/*
    int index = _seq%128;

    cs_local[index].seq = _seq;
    cs_local[index].cs = _cs;
    cs_local[index].theta = _theta;
    cs_local[index].phi = _phi;

    //printf("cs_seq= %i, _seq= %i \n", cs_seq, _seq);

    client_side_prediction_tick();

    //printf("control state= %i\n", new_control_state);
*/
}

void client_side_prediction_tick()
{
/*
    Agent_state* A = ClientState::agent_list.get(agent_id);
    if(A == NULL) return;

    struct Agent_control_state _cs;

    _cs = cs[cs_seq_local % 128];

    s = _agent_tick(_cs, box, s);
*/
}

void PlayerAgent_state::handle_net_control_state(int _seq, int _cs, float _theta, float _phi) {

    int index = _seq%128;

    //save cs
    cs_net[index].seq = _seq;
    cs_net[index].cs = _cs;
    cs_net[index].theta = _theta;
    cs_net[index].phi = _phi;

    //clear out old cs
    int i;
    for(i=32;i<64;i++){
        index = (_seq + i)%128;
        cs_net[index].seq = -1;
    }
    //check for differences between client out cs and in??

    if(cs_net[index].seq == -1) return;

    if(cs_net[index].seq != cs_local[index].seq) printf("player agent: e1\n");
    if(cs_net[index].cs != cs_local[index].cs) printf("player agent: server corrected control state\n");
    if(cs_net[index].theta != cs_local[index].theta) printf("player agent: e3\n");
    if(cs_net[index].phi != cs_local[index].phi) printf("player agent: e4\n");
}

//set actually sends
void PlayerAgent_state::set_control_state(uint16_t cs, float theta, float phi) {
    if(agent_id == -1) return;  //player agent not set

    cs_seq_local = (cs_seq_local+1) % 256;

    //printf("set cs= %i \n", cs_seq_local);

    Agent_cs_CtoS csp;

    csp.id = agent_id;
    csp.seq = cs_seq_local;
    csp.cs = cs;
    csp.theta = theta;
    csp.phi = phi;

    //printf("send cs= %i \n", cs_seq_local);
    csp.send();

    //add control state to control buffer
    
    //handle_local_control_state(cs_seq_local, cs, theta, phi);

    //save control state
    int index = cs_seq_local%128;

    cs_local[index].seq = cs_seq_local;
    cs_local[index].cs = cs;
    cs_local[index].theta = theta;
    cs_local[index].phi = phi;
    //clear out next control state
    cs_local[(index+1)%128].seq = -1;
    //client side tick forward

    //printf("0 _cs seq= %i \n", index);

    Agent_state* A = ClientState::agent_list.get(agent_id);
    if(A == NULL) return;

    //working version
    struct Agent_control_state _cs;
    //printf("0 index= %i \n", index);
    _cs = cs_local[index];

    class AgentState s_tmp = s;

    s = _agent_tick(_cs, A->box, s);

    //experimental
    //tick from last received snapshot to current

    // (state_history_index+1)%AGENT_STATE_HISTORY_SIZE

/*
    int csindex = (state_history_seq-3+256) % 256;
    class AgentState tmp = state_history[ (state_history_seq-3+AGENT_STATE_HISTORY_SIZE) % AGENT_STATE_HISTORY_SIZE ];
*/
    int csindex = (state_history_seq) % 256;
    class AgentState tmp = state_history[ state_history_index ];
    //printf("l seq= %i, tseq= %i, z= %f \n", tmp.seq, tmp.seq, tmp.z);

    //int stop_index = (cs_seq_local-1+256) % 256;
    int stop_index = cs_seq_local;


    //printf("start index= %i, stop index= %i \n", csindex, stop_index);
    int _i = 0;
    while(1)
    {
        _i++;
        if(csindex == stop_index) break;

        _cs = cs_local[csindex%128]; //check seq number

        tmp = _agent_tick(_cs, A->box, tmp);
        //printf("r  seq= %i, tseq= %i, cs= %i, z= %f \n", _cs.seq, tmp.seq, _cs.cs, tmp.z);
        tmp.seq = (tmp.seq+1) % 256;

        csindex = (csindex+1) % 256;
    }
    //if(_i > 5) printf("i= %i \n", _i);

    //printf("1 _cs seq= %i \n", _cs.seq % 128);

    //_cs = cs_local[csindex%128];
    //tmp = _agent_tick(_cs, A->box, tmp);
    //tmp.seq = (tmp.seq+1) % 256;


    s0 = tmp;
    //printf("f1 seq= %i, tseq= %i, cs= %i, z= %f \n", _cs.seq, tmp.seq, _cs.cs, tmp.z);

    //printf("2 _cs seq= %i \n", _cs.seq % 128);
    //printf("2 index= %i \n", cs_seq_local % 128);

    //printf("s x,y,z = %f, %f, %f \n", s.x,s.y,s.z);
    //printf("t x,y,z = %f, %f, %f \n", tmp.x, tmp.y, tmp.z);

    //if( (cs_seq_local % 128) != ((csindex+1)%128) ) printf("SAGSDAGASG !!! \n");
     
    _cs = cs_local[cs_seq_local % 128];
    s1 = _agent_tick(cs_local[cs_seq_local % 128], A->box, tmp);
    
    //tmp.seq = (tmp.seq+1) % 256;
    //printf("f2 seq= %i, tseq= %i, cs= %i, z= %f \n", _cs.seq, tmp.seq, _cs.cs, s1.z);
}


float PlayerAgent_state::camera_height() {
    
    //Agent_state has not crouching parameter!!! Crouching must be in player agent
    //if (active_camera_state->crouching) {
    //    return box.c_height * camera_height_scale;
    //}
    Agent_state* a = ClientState::agent_list.get(agent_id);
    if (a==NULL) return 0.0f;
    return a->camera_height();
}

#define INITIAL_AGENT_INTERPOLATION_WEIGHT 1.0f
static inline float _agent_weight(float t) {
    return INITIAL_AGENT_INTERPOLATION_WEIGHT * pow(AGENT_INTERPOLATION_DECAY, t/TICK_DURATION);
}

static inline float _agent_interp(float s0, float s1, float scale) {
    return scale*(s1 - s0) + s0;
}

// assumes constant time between history states, until delta_t is defined on the states
void PlayerAgent_state::update_camera_smoothing() {
    smooth.x = 0.0f;
    smooth.y = 0.0f;
    smooth.z = 0.0f;
    smooth.vx = 0.0f;
    smooth.vy = 0.0f;
    smooth.vz = 0.0f;

    AgentState* a = &smooth;
    float divisor = 0.0f;
    float t = 0.0f;
    float weight;
    
    int i, index;
    for (i=0; i<AGENT_STATE_HISTORY_SIZE; i++) {
        index = (AGENT_STATE_HISTORY_SIZE + state_history_index - i) % AGENT_STATE_HISTORY_SIZE;

        weight = _agent_weight(t);
        divisor += weight;

        a->x += state_history[index].x * weight;
        a->y += state_history[index].y * weight;
        a->z += state_history[index].z * weight;
        a->vx += state_history[index].vx * weight;
        a->vy += state_history[index].vy * weight;
        a->vz += state_history[index].vz * weight;

        t += TICK_DURATION;
        //t += state_history[index].dt;
    }

    a->x /= divisor;
    a->y /= divisor;
    a->z /= divisor;
    a->vx /= divisor;
    a->vy /= divisor;
    a->vz /= divisor;

#ifdef DC_CLIENT // whole file should be ifdef'd?
    Agent_state* A = ClientState::agent_list.get(agent_id);
    if (A==NULL) return;
    c.theta = A->s.theta;
    c.phi = A->s.phi;
#endif
}

void PlayerAgent_state::update_sound() {
    AgentState s = camera_state;

    float f[3];
    s.forward_vector(f);

    Sound::update_listener(s.x, s.y, s.z, s.vx, s.vy, s.vz, f[0], f[1], f[2], 0,0,1);
}

#endif
