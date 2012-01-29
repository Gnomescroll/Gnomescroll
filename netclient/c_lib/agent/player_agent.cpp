#pragma once

#include "player_agent.hpp"

#include <c_lib/agent/agent.hpp>
#include <c_lib/ray_trace/ray_trace.h>

#ifdef DC_CLIENT
#include <c_lib/sound/sound.hpp>
//#endif

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
    this->you = ClientState::agent_list.get(id);
    if (this->you == NULL)
    {
        printf("WARNING: set_PlayerAgent_id -- agent %d does not exist\n", id);
    }
    this->agent_id = id;
}


void PlayerAgent_state::update_client_side_prediction_interpolated()
{
    int last_tick = _LAST_TICK();
    int _t = _GET_MS_TIME();

    //printf("ms since last update= %i \n", _t - last_tick);
    float delta = ((float)(_t - last_tick)) / 33.3f;

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
    if (this->you == NULL) return;
    c.theta = this->you->s.theta;
    c.phi = this->you->s.phi;
#endif
}

void PlayerAgent_state::handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz) {

    class AgentState ss;

    ss.seq = seq;
    ss.theta = theta;
    ss.phi = phi;
    ss.x=x;ss.y=y;ss.z=z;
    ss.vx=vx;ss.vy=vy;ss.vz=vz;

    int index = seq%AGENT_STATE_HISTORY_SIZE;

    state_history[index] = ss;

    if( (state_history_seq - seq) > 30 || seq > state_history_seq) {
        state_history_index = index;
        state_history_seq = seq; //set index
        state_snapshot = ss;    //set state snapsot
    }

}

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

uint16_t PlayerAgent_state::pack_control_state(
    int f, int b, int l, int r,
    int jet, int jump, int crouch, int boost,
    int misc1, int misc2, int misc3
)
{
    uint16_t cs = 0;
    if(f) cs |= 1;
    if(b) cs |= 2;
    if(l) cs |= 4;
    if(r) cs |= 8;
    if(jet) cs |= 16;
    if(jump) cs |= 32;
    if(crouch) cs |= 64;
    if(boost) cs |= 128;
    if(misc1) cs |= 256;
    if(misc2) cs |= 512;
    if(misc3) cs |= 1024;
    return cs;
}

uint16_t PlayerAgent_state::sanitize_control_state(uint16_t cs)
{
    //set control state variables
    int forward     = cs & 1? 1 :0;
    int backwards   = cs & 2? 1 :0;
    int left        = cs & 4? 1 :0;
    int right       = cs & 8? 1 :0;
    int jetpack     = cs & 16? 1 :0;
    int jump        = cs & 32? 1 :0;
    int crouch      = cs & 64? 1 :0;
    int boost       = cs & 128? 1 :0;
    int misc1       = cs & 256? 1 :0;
    int misc2       = cs & 512? 1 :0;
    int misc3       = cs & 1024? 1 :0;     

    AgentState* state;
    //state = &this->you->s;
    state = &this->s0;

    // force staying crouched if cant stand up
    if ((this->crouching && !crouch)
    && collision_check5(
        this->you->box.box_r, this->you->box.b_height,
        state->x, state->y, state->z
    ))
    {
        crouch = 1;
    }
    this->crouching = (bool)crouch;

    // only jump if on ground
    if (jump && !on_ground(this->you->box.box_r, state->x, state->y, state->z))
    {
        jump = 0;
    }

    cs = this->pack_control_state(
        forward, backwards, left, right,
        jetpack, jump, crouch, boost,
        misc1, misc2, misc3
    );
    return cs;
}

void PlayerAgent_state::set_control_state(int f, int b, int l, int r, int jet, int jump, int crouch, int boost, int misc1, int misc2, int misc3, float theta, float phi)
{
    uint16_t cs;
    cs = this->pack_control_state(f,b,l,r,jet,jump,crouch,boost,misc1,misc2,misc3);
    cs = this->sanitize_control_state(cs);
    this->set_control_state(cs, theta, phi);
}

//set actually sends
void PlayerAgent_state::set_control_state(uint16_t cs, float theta, float phi) {
    if(this->you == NULL) return;  //player agent not set

    cs = this->sanitize_control_state(cs);

    cs_seq_local = (cs_seq_local+1) % 256;

    Agent_cs_CtoS csp;

    csp.seq = cs_seq_local;
    csp.cs = cs;
    csp.theta = theta;
    csp.phi = phi;

    csp.send();

    //save control state
    int index = cs_seq_local%128;

    cs_local[index].seq = cs_seq_local;
    cs_local[index].cs = cs;
    cs_local[index].theta = theta;
    cs_local[index].phi = phi;
    //clear out next control state
    cs_local[(index+1)%128].seq = -1;
    //client side tick forward

    struct Agent_control_state acs;
    acs = cs_local[index];

    s = _agent_tick(acs, you->box, s, this->you);

    int cs_index = (state_history_seq) % 256;
    class AgentState tmp = state_history[ state_history_index ];

    int stop_index = cs_seq_local;

    while (cs_index != stop_index)
    {
        acs = cs_local[cs_index%128]; //check seq number

        tmp = _agent_tick(acs, you->box, tmp, this->you);
        tmp.seq = (tmp.seq+1) % 256;

        cs_index = (cs_index+1) % 256;
    }
    s0 = tmp;
    acs = cs_local[cs_seq_local % 128];
    s1 = _agent_tick(cs_local[cs_seq_local % 128], you->box, tmp, this->you);
}

float PlayerAgent_state::camera_height() {
    if (this->you == NULL) return 0.0f;
    return this->you->camera_height();
}

void PlayerAgent_state::display_agent_names()
{
    #ifdef DC_CLIENT
    if (this->you == NULL) return;
    float threshold = (3.14159 / 180) * 18; //degrees->radians
    AgentState *s = &this->camera_state;
    float f[3];
    this->camera_state.forward_vector(f);
    ClientState::agent_list.agents_in_cone(
        s->x, s->y, s->z + this->camera_height(),
        f[0], f[1], f[2],
        threshold
    );

    // hide all names
    for (int i=0; i < ClientState::agent_list.n_max; i++)
    {
        Agent_state* a = ClientState::agent_list.a[i];
        if (a==NULL) continue;
        a->event.hide_name();
    }

    // choose names to display
    for (int i=0; i < ClientState::agent_list.n_filtered; i++)
    {
        Agent_state* a = ClientState::agent_list.filtered_agents[i];
        if (a==NULL) continue;
        if (a->id == this->agent_id) continue;
        if (a->status.team != you->status.team) continue;  // only show teammates
        a->event.display_name();
    }
    #endif
}

void PlayerAgent_state::update_sound() {
    AgentState s = camera_state;

    float f[3];
    s.forward_vector(f);

    Sound::update_listener(s.x, s.y, s.z, s.vx, s.vy, s.vz, f[0], f[1], f[2], 0,0,1);
}

PlayerAgent_state::PlayerAgent_state()
:
action(this)
{
    //init
    static int inited=0;
    if (inited) printf("WARNING Only one PlayerAgent_state should exist\n");
    inited++;

    state_history = new AgentState[AGENT_STATE_HISTORY_SIZE];

    //client side state variables
    crouching = false;
    //camera
    camera_mode = client_side_prediction_interpolated;

    agent_id = -1;
    this->you = NULL;
    
    cs_seq_local = 0;
    cs_seq_net = -1;

    state_history_index = 0;
    state_history_seq = 0;

    for(int i=0; i<128; cs_local[i++].seq = -1);
    for(int i=0; i<128; cs_net[i++].seq = -1) ;
}

PlayerAgent_state::~PlayerAgent_state()
{
    if (this->state_history != NULL)
        delete[] this->state_history;
}

void PlayerAgent_state::toggle_camera_mode() {
    camera_mode = (camera_mode + 1) % CameraStatesEnd;
    switch (camera_mode) {
        case net_agent:
            printf("Camera Mode: net_agent\n");
            break;
        case client_side_prediction_interpolated:
            printf("Camera Mode: client_side_prediction_interpolated\n");
            break;
        case client_side_prediction:
            printf("Camera Mode: client_side_prediction\n");
            break;
        case last_server_snapshot:
            printf("Camera Mode: last_server_snapshot\n");
            break;
        default:
            printf("PlayerAgent toggle_camera_mode: error\n");
            break;
        //active_camera_state = &camera_state;
    }
}

void PlayerAgent_state::pump_camera() {
    switch (camera_mode) {
        case net_agent:
            #ifdef DC_CLIENT
            if(agent_id != -1) {
                Agent_state* A = ClientState::agent_list.get(agent_id);
                camera_state = A->s;
            } else {
                printf("PlayerAgent Camera: cannot pump net_agent camera; agent does not exist");
            }
            #endif
            break;
        case client_side_prediction_interpolated:
            update_client_side_prediction_interpolated();
            camera_state = c;
            break;
        case client_side_prediction:
            camera_state = s;
            break;
        case last_server_snapshot:
            camera_state = state_snapshot;
            break;
        default:
            printf("PlayerAgent toggle_camera_mode: error\n");
            break;
    }
}



#endif
