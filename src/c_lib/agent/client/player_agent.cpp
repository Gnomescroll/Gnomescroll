#include "player_agent.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#if DC_CLIENT
#include <state/client_state.hpp>
#endif
#include <agent/agent.hpp>
#include <physics/ray_trace/ray_trace.hpp>
#include <sound/sound.hpp>
#include <common/time/physics_timer.hpp>
#include <input/handlers.hpp>
#include <chat/interface.hpp>
#include <t_map/_interface.hpp>
#include <agent/client/agent_sound_handler.hpp>

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

//#define SKIP_INTERPOLATION_THRESHOLD 0.5f // travel distance above which we dont bother interpolating to

void PlayerAgent_state::update_client_side_prediction_interpolated()
{
    static const float SKIP_INTERPOLATION_THRESHOLD = 1.0f;
    static int _tl;

    // calculate interpolation delta
    // if the distance travelled is extreme, dont interpolate (delta=1)
    // this is because of the teleport in map wrapping
    // the player will suddenly jump ~512 meters, with a noticable interpolation flicker in between
    float dist;
    {
        float x0 = quadrant_translate_f(current_camera_position.x, s0.x);
        float y0 = quadrant_translate_f(current_camera_position.y, s0.y);
        float z0 = s0.z;

        float x1 = quadrant_translate_f(current_camera_position.x, s1.x);
        float y1 = quadrant_translate_f(current_camera_position.y, s1.y);
        float z1 = s1.z;

        dist = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1));
    }

    float delta = 1.0f;
    if (dist < SKIP_INTERPOLATION_THRESHOLD)
    {
        delta = ((float)(_t - last_tick)) / 33.0;
        if(delta > 1.0f) delta = 1.0f;

    }
    else
    {
        delta = 1.0f;
    }

    GS_ASSERT(delta >= 0.0f);

    c.x = s0.x*(1-delta) + s1.x*delta;
    c.y = s0.y*(1-delta) + s1.y*delta;
    c.z = s0.z*(1-delta) + s1.z*delta;  

#if 0
    int _tl0 = _GET_MS_TIME() -_tl;
    _tl = _GET_MS_TIME();
    int last_tick = (int)_LAST_TICK();
    int _t = (int)_GET_MS_TIME();

    static float lz;
    static float lz0;
    printf("z= %0.02f delta= %0.02f s0.z= %0.02f s0_delta= %0.02f   tdelta= %0.02f dist= %f tickd= %d time= %d ctime= %d last_tick= %d\n", 
        c.z, lz - c.z, s0.z, lz0-s0.z, delta, dist, _t - last_tick, _tl0, _GET_MS_TIME(), _LAST_TICK());
    lz = c.z;
    lz0 = s0.z;
#endif

#if 1
    if (this->you == NULL) return;
    AgentState s = this->you->get_state();
    c.theta = s.theta;
    c.phi = s.phi;
#else
    //AgentState s = this->you->get_state();

    /*
        This may introduce errors if this is used
    */
    c.theta = 0.0f;
    c.phi   = 0.0f;
#endif
}

void PlayerAgent_state::handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz)
{
    class AgentState ss;

    ss.seq = seq;
    ss.theta = theta;
    ss.phi = phi;
    ss.x=x;ss.y=y;ss.z=z;
    ss.vx=vx;ss.vy=vy;ss.vz=vz;

    int index = seq % AGENT_STATE_HISTORY_SIZE;

    state_history[index] = ss;

    if( (state_history_seq - seq) > 30 || seq > state_history_seq)
    {
        state_history_index = index;
        state_history_seq = seq; //set index
        state_snapshot = ss;    //set state snapsot
    }
}

void PlayerAgent_state::handle_net_control_state(int _seq, int _cs, float _theta, float _phi)
{
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

    AgentState s = this->you->get_state();
    if (this->you->status.dead)
    {   // use the last agent state's theta,phi (instead of the camera which is normally passed in)
        theta = s.theta;
        phi = s.phi;
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

    // tick sound motion
    bool is_on_ground = on_ground(this->you->box.box_r, camera_state.x, camera_state.y, camera_state.z);
    player_agent_sound_ground_movement_event(s0, s1, is_on_ground);
}

float PlayerAgent_state::camera_height()
{
    if (this->you == NULL) return 0.0f;
    return this->you->camera_height();
}

float PlayerAgent_state::camera_z()
{
    if (this->you == NULL) return 0.0f;
    return this->camera_state.z + this->camera_height();
}

Vec3 PlayerAgent_state::camera_position()
{
    if (this->you == NULL) return vec3_init(0,0,0);
    return vec3_init(this->camera_state.x, this->camera_state.y, this->camera_z());
}

void PlayerAgent_state::update_sound()
{
    AgentState s = camera_state;
    Vec3 p = agent_camera->forward_vector();
    Sound::update_listener(s.x, s.y, s.z, s.vx, s.vy, s.vz, p.x, p.y, p.z, 0.0f,0.0f,1.0f);
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

void PlayerAgent_state::pump_camera()
{
    Agent_state* A;
    switch (camera_mode)
    {
        case net_agent:
            A = ClientState::agent_list->get(agent_id);
            if (A != NULL) camera_state = A->get_state();
            break;
        case client_side_prediction_interpolated:   // default
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
            GS_ABORT();
            break;
    }
}

void PlayerAgent_state::update_model()
{
    Agent_state* a = this->you;
    if (a == NULL) return;
    if (a->vox == NULL) return;

    Vec3 center;
    if (a->vox->was_updated)         
        center = a->get_center();    // model is fresh, use model center for more accurate culling
    else
        center = a->get_position();  // model is stale, must use agent position
        
    a->vox->was_updated = false;

    // other agents
    VoxDat* vox_dat = &VoxDats::agent;
    float radius = a->vox->get_part(0)->radius;
    if (sphere_fulstrum_test_translate(center.x, center.y, center.z, radius) == false)
    {   // agent not in view fulcrum
        a->vox->set_draw(false);
        a->vox->set_hitscan(false);
        return;
    }

    if (a->crouched())
    {
        vox_dat = &VoxDats::agent_crouched;
        if (!a->status.vox_crouched)
        {
            a->vox->set_vox_dat(vox_dat);
            a->vox->reset_skeleton();
            a->status.vox_crouched = true;
            a->event.set_agent_vox_status(AGENT_VOX_IS_CROUCHED);
        }
    }
    else
    {
        if (a->status.vox_crouched)
        {   // was crouched last frame, but not a frame: restore standing model
            a->vox->set_vox_dat(vox_dat);
            a->vox->reset_skeleton();
            a->status.vox_crouched = false;
            a->event.set_agent_vox_status(AGENT_VOX_IS_STANDING);
        }
    }
    if (a->status.dead)
    {
        vox_dat = &VoxDats::agent_dead;
        a->event.set_agent_vox_status(AGENT_VOX_IS_DEAD);
    }
        
    a->vox->set_vox_dat(vox_dat);
    a->update_legs();
    AgentState s = a->get_state();
    a->vox->update(s.x, s.y, s.z, s.theta, -s.phi);
    if (current_camera->first_person)
    {
        a->vox->set_draw(false);
        a->vox->set_hitscan(false);
    }
    else
    {
        a->vox->set_draw(true);
        a->vox->set_hitscan(true);
    }
}

int* PlayerAgent_state::facing_block()
{
    if (this->you == NULL) return NULL;
    if (current_camera == NULL) return NULL;

    const float max_distance = 4.0f;
    const int z_low = 4;
    const int z_high = 3;

    Vec3 direction = current_camera->forward_vector();
    int* pos = _nearest_block(current_camera_position, direction, max_distance, z_low, z_high);
    return pos;
}

int PlayerAgent_state::facing_container()
{
    int* pos = this->facing_block();
    if (pos == NULL) return NULL_CONTAINER;

    return t_map::get_block_item_container(pos[0], pos[1], pos[2]);
}

int* PlayerAgent_state::nearest_open_block(const float max_dist, const int z_low, const int z_high)
{
    if (agent_camera == NULL) return NULL;
    Vec3 f = agent_camera->forward_vector();
    ASSERT_BOXED_POSITION(this->camera_state);
    int* b = _farthest_empty_block(
        this->camera_state.x, this->camera_state.y, this->camera_z(),
        f.x, f.y, f.z,
        max_dist, z_low, z_high);
    return b;
}

int PlayerAgent_state::get_facing_side(int solid_pos[3], int open_pos[3], int side[3], float* distance)
{
    if (agent_camera == NULL) return 0;
    Vec3 p = agent_camera->get_position();
    Vec3 v = agent_camera->forward_vector();
    int tile = 0;
    Hitscan::HitscanTargetTypes target = Hitscan::terrain(p.x, p.y, p.z, v.x, v.y, v.z, solid_pos, distance, side, &tile);
    if (target != Hitscan::HITSCAN_TARGET_BLOCK) return 0;
    open_pos[0] = translate_point(solid_pos[0] + side[0]);
    open_pos[1] = translate_point(solid_pos[1] + side[1]);
    open_pos[2] = solid_pos[2] + side[2];
    GS_ASSERT(open_pos[2] >= 0); // agent should never be shooting from underground
    GS_ASSERT(tile > 0);
    return tile;
}

int PlayerAgent_state::get_facing_side(int solid_pos[3], int open_pos[3], float* distance)
{
    int s[3];
    int block = this->get_facing_side(solid_pos, open_pos, s, distance);
    if (block <= 0)
        return -1;
    return get_cube_side_from_side_array(s);
}


