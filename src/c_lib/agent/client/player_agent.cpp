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
#include <chat/_interface.hpp>
#include <t_map/_interface.hpp>
#include <agent/client/agent_sound_handler.hpp>

class Agent* PlayerAgent_state::you()
{
    if (this->agent_id == NULL_AGENT) return NULL;
    return Agents::agent_list->get(this->agent_id);
}

void PlayerAgent_state::set_PlayerAgent_id(AgentID id)
{
    GS_ASSERT(id != NULL_AGENT);
    this->agent_id = id;
    GS_ASSERT(this->you() != NULL);
    input_state.input_mode = INPUT_STATE_AGENT;
    input_state.camera_mode = INPUT_STATE_AGENT;
    Chat::subscribe_channels();
}

/*
    ERROR INTERPOLATION IS STILL WRONG: TIME DELTA IS INVALID
*/
void PlayerAgent_state::update_client_side_prediction_interpolated()
{    
    int _t = (int)_GET_MS_TIME();
    int last_tick = (int)_LAST_TICK();

    struct Vec3 vs0 = s0.get_position();
    struct Vec3 vs1 = s1.get_position();

    vs0 = quadrant_translate_position(current_camera_position, vs0);
    vs1 = quadrant_translate_position(current_camera_position, vs1);

    float dist = vec3_distance(vs0, vs1);

    // calculate interpolation delta
    // if the distance travelled is extreme, dont interpolate (delta=1)
    // this is because of the teleport in map wrapping
    // the player will suddenly jump ~512 meters, with a noticable interpolation flicker in between
    static const float SKIP_INTERPOLATION_THRESHOLD = 1.0f;
    float delta = 1.0f;
    if (dist < SKIP_INTERPOLATION_THRESHOLD)
    {
        delta = ((float)(_t - last_tick)) / 33.0f;
        if(delta > 1.0f) delta = 1.0f;
        GS_ASSERT(delta >= 0.0f);
        if (delta < 0.0f) delta = 0.0f;
    }

    vs0 = vec3_scalar_mult(vs0, 1.0f-delta);
    vs1 = vec3_scalar_mult(vs1, delta);
    struct Vec3 v = vec3_add(vs0, vs1);
    v = translate_position(v);
    c.set_position(v);

    class Agent* a = this->you();
    if (a != NULL)
    {
        AgentState s = a->get_state();
        c.theta = s.theta;
        c.phi = s.phi;
    }
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
    int misc1, int misc2, int misc3)
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
    class Agent* a = this->you();
    if (a == NULL) return 0;
    if (a->status.dead) return 0;

    int forward,backwards,left,right,jp,jump,crouch,boost,misc1,misc2,misc3;
    //set control state variables
    forward     = cs & CS_FORWARD ? 1 :0;
    backwards   = cs & CS_BACKWARD ? 1 :0;
    left        = cs & CS_LEFT ? 1 :0;
    right       = cs & CS_RIGHT ? 1 :0;
    jp          = cs & CS_JETPACK ? 1 :0;
    jump        = cs & CS_JUMP ? 1 :0;
    crouch      = cs & CS_CROUCH ? 1 :0;
    boost       = cs & CS_BOOST ? 1 :0;
    misc1       = cs & CS_MISC1 ? 1 :0;
    misc2       = cs & CS_MISC2 ? 1 :0;
    misc3       = cs & CS_MISC3 ? 1 :0;

    AgentState* state;
    state = &this->s1;

    // force staying crouched if cant stand up
    if ((this->crouching && !crouch) && can_stand_up(a->box.box_r, a->box.b_height,
                                                        state->x, state->y, state->z))
    {
        crouch = 1;
    }
    this->crouching = (bool)crouch;

    // only jump if on ground
    if (jump && !on_ground(a->box.box_r, state->x, state->y, state->z))
    {
        jump = 0;
    }

    cs = this->pack_control_state(
        forward, backwards, left, right,
        jetpack.update(jp),
        jump, crouch, boost,
        misc1, misc2, misc3);
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
void PlayerAgent_state::set_control_state(uint16_t cs, float theta, float phi)
{
    class Agent* a = this->you();
    if(a == NULL) return;  //player agent not set

    cs = this->sanitize_control_state(cs);

    cs_seq_local = (cs_seq_local+1) % 256;

    AgentState s = a->get_state();
    if (a->status.dead)
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

    struct Agent_control_state acs = cs_local[index];

    int cs_index = (state_history_seq) % 256;
    class AgentState tmp = state_history[state_history_index];

    int stop_index = cs_seq_local;

    while (cs_index != stop_index)
    {
        acs = cs_local[cs_index%128]; //check seq number

        tmp = _agent_tick(acs, a->box, tmp);
        tmp.seq = (tmp.seq+1) % 256;

        cs_index = (cs_index+1) % 256;
    }
    s0 = tmp;
    acs = cs_local[cs_seq_local % 128];
    s1 = _agent_tick(acs, a->box, s0);

    // tick sound motion
    bool s1_on_ground = on_ground(a->box.box_r, s1.x, s1.y, s1.z);
    bool camera_on_ground = on_ground(a->box.box_r, camera_state.x, camera_state.y, camera_state.z);
    //if (s0.vx || s0.vy || s0.vz || s1.vx || s1.vy || s1.vz)
    //{
        //printf("s0 ");s0.print();
        //printf("s1 ");s1.print();
        //printf("--------------------------\n");
    //}
    player_agent_sound_ground_movement_event(s0, s1, s1_on_ground, camera_on_ground);
}

float PlayerAgent_state::camera_height()
{
    class Agent* a = this->you();
    if (a == NULL) return 0.0f;
    return a->camera_height();
}

float PlayerAgent_state::camera_z()
{
    if (this->you() == NULL) return 0.0f;
    return this->camera_state.z + this->camera_height();
}

struct Vec3 PlayerAgent_state::camera_position()
{
    if (this->you() == NULL) return vec3_init(0,0,0);
    return vec3_init(this->camera_state.x, this->camera_state.y, this->camera_z());
}

void PlayerAgent_state::update_sound()
{
    AgentState s = camera_state;
    Vec3 p = agent_camera->forward_vector();
    Sound::update_listener(s.x, s.y, s.z, s.vx, s.vy, s.vz, p.x, p.y, p.z, 0.0f,0.0f,1.0f);
}

PlayerAgent_state::PlayerAgent_state() :
    crouching(false), camera_mode(client_side_prediction_interpolated),
    cs_seq_local(255), cs_seq_net(-1),
    state_history_seq(0), state_history_index(0),
    agent_id(NULL_AGENT),  action(this)
{
    state_history = new AgentState[AGENT_STATE_HISTORY_SIZE];
    for(int i=0; i<128; cs_local[i++].seq = -1);
    for(int i=0; i<128; cs_net[i++].seq = -1) ;
}

PlayerAgent_state::~PlayerAgent_state()
{
    if (this->state_history != NULL)
        delete[] this->state_history;
}

void PlayerAgent_state::toggle_camera_mode()
{
    camera_mode = (camera_mode + 1) % CameraStatesEnd;
    switch (camera_mode)
    {
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
    Agent* A;
    switch (camera_mode)
    {
        case net_agent:
            A = Agents::get_agent(agent_id);
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
    class Agent* a = this->you();
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

bool PlayerAgent_state::facing_block(int pos[3])
{
    if (this->you() == NULL) return NULL;
    if (current_camera == NULL) return NULL;

    const float max_distance = 4.0f;

    struct RaytraceData data;
    bool collided = raytrace_terrain(current_camera_position, current_camera->forward_vector(), max_distance, &data);
    if (!collided) return false;
    for (int i=0; i<3; i++)
        pos[i] = data.collision_point[i];
    return collided;
}

int PlayerAgent_state::facing_container()
{
    int pos[3];
    bool facing = this->facing_block(pos);
    if (!facing) return NULL_CONTAINER;

    return t_map::get_block_item_container(pos[0], pos[1], pos[2]);
}

bool PlayerAgent_state::nearest_open_block(const float max_dist, int open_point[3])
{
    if (agent_camera == NULL) return NULL;
    struct Vec3 f = agent_camera->forward_vector();

    struct RaytraceData data;
    bool collided = raytrace_terrain(agent_camera->get_position(), f, max_dist, &data);
    if (!collided) return false;
    data.get_pre_collision_point(open_point);
    return collided;
}

int PlayerAgent_state::get_facing_side(int solid_pos[3], int open_pos[3], int side[3], float* distance)
{
    if (agent_camera == NULL) return 0;
    struct Vec3 p = agent_camera->get_position();
    struct Vec3 v = agent_camera->forward_vector();

    const float max_dist = 128.0f;
    struct RaytraceData data;
    bool collided = raytrace_terrain(p, v, max_dist, &data);
    if (!collided) return 0;

    for (int i=0; i<3; i++) solid_pos[i] = data.collision_point[i];
    data.get_pre_collision_point(open_pos);
    data.get_side_array(side);
    *distance = data.interval * max_dist;
    
    GS_ASSERT(open_pos[2] >= 0); // agent should never be shooting from underground
    return data.get_cube_id();
}

int PlayerAgent_state::get_facing_side(int solid_pos[3], int open_pos[3], float* distance)
{
    int s[3];
    int block = this->get_facing_side(solid_pos, open_pos, s, distance);
    if (block <= 0)
        return -1;
    return get_cube_side_from_side_array(s);
}

#if !PRODUCTION
void PlayerAgent_state::teleport_to(struct Vec3 p)
{
    teleport_me_CtoS msg;
    msg.position = p;
    msg.send();
}
#endif
