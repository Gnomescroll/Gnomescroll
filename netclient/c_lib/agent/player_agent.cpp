#pragma once

#include "player_agent.hpp"

#ifdef DC_CLIENT

#include <c_lib/agent/agent.hpp>
#include <c_lib/ray_trace/ray_trace.hpp>
#include <c_lib/sound/sound.hpp>

#include <c_lib/time/physics_timer.h>
#include <c_lib/state/client_state.hpp>
#include <c_lib/input/handlers.hpp>
#include <c_lib/chat/client.hpp>

void PlayerAgent_state::set_PlayerAgent_id(int id) {
    this->you = ClientState::agent_list->get(id);
    if (this->you == NULL)
        printf("ERROR: set_PlayerAgent_id -- agent %d does not exist\n", id);
    this->agent_id = id;
    input_state.input_mode = INPUT_STATE_AGENT;
    input_state.camera_mode = INPUT_STATE_AGENT;
    chat_client->subscribe_channels();
}

void PlayerAgent_state::was_identified()
{
    this->identified = true;
    request_remaining_state_CtoS msg;
    msg.send();    
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

    if (this->you == NULL) return;
    c.theta = this->you->s.theta;
    c.phi = this->you->s.phi;
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

void PlayerAgent_state::handle_net_control_state(int _seq, int _cs, float _theta, float _phi) {

    int index = _seq%128;

    //save cs
    cs_net[index].seq = _seq;
    cs_net[index].cs = _cs;
    cs_net[index].theta = _theta;
    cs_net[index].phi = _phi;

    //clear out old cs
    for(int i=32;i<64;i++){
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
    if(f) cs |= CS_FORWARD;
    if(b) cs |= CS_BACKWARD;
    if(l) cs |= CS_LEFT;
    if(r) cs |= CS_RIGHT;
    if(jet) cs |= CS_JETPACK;
    if(jump) cs |= CS_JUMP;
    if(crouch) cs |= CS_CROUCH;
    if(boost) cs |= CS_BOOST;
    if(misc1) cs |= CS_MISC1;
    if(misc2) cs |= CS_MISC2;
    if(misc3) cs |= CS_MISC3;
    return cs;
}

uint16_t PlayerAgent_state::sanitize_control_state(uint16_t cs)
{
    if (this->you == NULL) return 0;
    if (this->you->status.dead) return 0;

    int forward,backwards,left,right,jetpack,jump,crouch,boost,misc1,misc2,misc3;
    //set control state variables
    forward     = cs & CS_FORWARD ? 1 :0;
    backwards   = cs & CS_BACKWARD ? 1 :0;
    left        = cs & CS_LEFT ? 1 :0;
    right       = cs & CS_RIGHT ? 1 :0;
    jetpack     = cs & CS_JETPACK ? 1 :0;
    jump        = cs & CS_JUMP ? 1 :0;
    crouch      = cs & CS_CROUCH ? 1 :0;
    boost       = cs & CS_BOOST ? 1 :0;
    misc1       = cs & CS_MISC1 ? 1 :0;
    misc2       = cs & CS_MISC2 ? 1 :0;
    misc3       = cs & CS_MISC3 ? 1 :0;

    AgentState* state;
    state = &this->s1;

    // force staying crouched if cant stand up
    if ((this->crouching && !crouch)
    && can_stand_up(
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

    if (jetpack_decay)
    {
        jetpack = 0;
        jetpack_decay--;
        jetpack_decay = (jetpack_decay < 0) ? 0 : jetpack_decay;
    }
    else
    {
        if (jetpack)
        {
            jetpack_ticks++;
            if (jetpack_ticks >= JETPACK_TICKS_MAX)
            {
                jetpack = 0;
                jetpack_ticks = 0;
                jetpack_decay = JETPACK_DECAY_TICKS;
            }
        }
        else
        {
            jetpack_ticks = (jetpack_ticks <= 0) ? 0 : jetpack_ticks-1;
        }
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

    if (this->you->status.dead)
    {   // use the last agent state's theta,phi (instead of the camera which is normally passed in)
        theta = this->you->s.theta;
        phi = this->you->s.phi;
    }

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

    int cs_index = (state_history_seq) % 256;
    class AgentState tmp = state_history[ state_history_index ];

    int stop_index = cs_seq_local;

    while (cs_index != stop_index)
    {
        acs = cs_local[cs_index%128]; //check seq number

        tmp = _agent_tick(acs, you->box, tmp);
        tmp.seq = (tmp.seq+1) % 256;

        cs_index = (cs_index+1) % 256;
    }
    s0 = tmp;
    acs = cs_local[cs_seq_local % 128];
    s1 = _agent_tick(cs_local[cs_seq_local % 128], you->box, tmp);
}

float PlayerAgent_state::camera_height() {
    if (this->you == NULL) return 0.0f;
    return this->you->camera_height();
}

float PlayerAgent_state::camera_z()
{
    if (this->you == NULL) return 0.0f;
    return this->camera_state.z + this->camera_height();
}

void PlayerAgent_state::display_agent_names()
{
    if (this->you == NULL) return;

    // hide all names
    for (int i=0; i < ClientState::agent_list->n_max; i++)
    {
        Agent_state* a = ClientState::agent_list->a[i];
        if (a==NULL) continue;
        a->event.hide_name();
    }

    // choose names to display
    for (int i=0; i < ClientState::agent_list->n_max; i++)
    {
        Agent_state* a = ClientState::agent_list->a[i];
        if (a==NULL) continue;
        if (a->id == this->agent_id)
        {
            a->event.hide_name();
            continue;
        }
        if (a->status.team != you->status.team) // only show teammates
        {
            a->event.hide_name();
            continue;
        }
        a->event.display_name();
    }
}

void PlayerAgent_state::update_sound() {
    AgentState s = camera_state;

    float f[3];
    agent_camera->forward_vector(f);

    Sound::update_listener(s.x, s.y, s.z, s.vx, s.vy, s.vz, f[0], f[1], f[2], 0,0,1);
}

PlayerAgent_state::PlayerAgent_state()
:
crouching(false),
jetpack_ticks(0),
jetpack_decay(0),
action(this),
identified(false)
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
    
    cs_seq_local = 255;  // so first one is zero
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
            if(agent_id != -1) {
                Agent_state* A = ClientState::agent_list->get(agent_id);
                camera_state = A->s;
            } else {
                printf("PlayerAgent Camera: cannot pump net_agent camera; agent does not exist");
            }
            break;
        case client_side_prediction_interpolated:
            update_client_side_prediction_interpolated();
            camera_state = c;
            break;
        case client_side_prediction:
            camera_state = s1;
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
