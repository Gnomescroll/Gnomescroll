#include "player_agent.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <state/client_state.hpp>
#include <agent/agent.hpp>
#include <physics/ray_trace/ray_trace.hpp>
#include <sound/sound.hpp>
#include <common/time/physics_timer.hpp>
#include <input/handlers.hpp>
#include <chat/_interface.hpp>
#include <t_map/_interface.hpp>

namespace Agents
{

class Agent* PlayerAgent::you()
{
    if (this->agent_id == NULL_AGENT) return NULL;
    return Agents::agent_list->get(this->agent_id);
}

void PlayerAgent::set_PlayerAgent_id(AgentID id)
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
void PlayerAgent::update_client_side_prediction_interpolated()
{
    int _t = int(_GET_MS_TIME());
    int last_tick = int(_LAST_TICK());

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
        delta = float(_t - last_tick) / 33.0f;
        if (delta > 1.0f) delta = 1.0f;
        IF_ASSERT(delta < 0.0f) delta = 0.0f;
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

void PlayerAgent::handle_state_snapshot(int seq, float theta, float phi,
                                        float x, float y, float z,
                                        float vx, float vy, float vz)
{
    class AgentState ss;
    ss.seq = seq;
    ss.theta = theta;
    ss.phi = phi;
    ss.x = x;
    ss.y = y;
    ss.z = z;
    ss.vx = vx;
    ss.vy = vy;
    ss.vz = vz;

    int index = seq % AGENT_STATE_HISTORY_SIZE;
    state_history[index] = ss;
    if ((state_history_seq - seq) > 30 || seq > state_history_seq)
    {
        state_history_index = index;
        state_history_seq = seq; //set index
        state_snapshot = ss;    //set state snapsot
    }
}

void PlayerAgent::handle_net_control_state(int _seq, int _cs, float _theta, float _phi)
{
    int index = _seq % 128;
    //save cs
    cs_net[index].seq = _seq;
    cs_net[index].cs = _cs;
    cs_net[index].theta = _theta;
    cs_net[index].phi = _phi;
    //clear out old cs
    for (int i=32; i<64; i++)
    {
        index = (_seq + i) % 128;
        cs_net[index].seq = -1;
    }
}

uint16_t PlayerAgent::pack_control_state(int f, int b, int l, int r,
                                         int jet, int jump, int crouch,
                                         int boost, int misc1, int misc2,
                                         int misc3)
{
    uint16_t cs = 0;
    if (f)      cs |= CS_FORWARD;
    if (b)      cs |= CS_BACKWARD;
    if (l)      cs |= CS_LEFT;
    if (r)      cs |= CS_RIGHT;
    if (jet)    cs |= CS_JETPACK;
    if (jump)   cs |= CS_JUMP;
    if (crouch) cs |= CS_CROUCH;
    if (boost)  cs |= CS_BOOST;
    if (misc1)  cs |= CS_MISC1;
    if (misc2)  cs |= CS_MISC2;
    if (misc3)  cs |= CS_MISC3;
    return cs;
}

uint16_t PlayerAgent::sanitize_control_state(uint16_t cs)
{
    class Agent* a = this->you();
    if (a == NULL) return 0;
    if (a->status.dead) return 0;
    // set control state variables
    int forward   = cs & CS_FORWARD  ? 1 : 0;
    int backwards = cs & CS_BACKWARD ? 1 : 0;
    int left      = cs & CS_LEFT     ? 1 : 0;
    int right     = cs & CS_RIGHT    ? 1 : 0;
    int jp        = cs & CS_JETPACK  ? 1 : 0;
    int jump      = cs & CS_JUMP     ? 1 : 0;
    int crouch    = cs & CS_CROUCH   ? 1 : 0;
    int boost     = cs & CS_BOOST    ? 1 : 0;
    int misc1     = cs & CS_MISC1    ? 1 : 0;
    int misc2     = cs & CS_MISC2    ? 1 : 0;
    int misc3     = cs & CS_MISC3    ? 1 : 0;

    AgentState* state = &this->s1;
    BoundingBox box = a->get_bounding_box();
    // force staying crouched if cant stand up
    if ((this->crouching && !crouch) &&
        can_stand_up(box.radius, box.height, state->x, state->y, state->z))
    {
        crouch = 1;
    }
    this->crouching = bool(crouch);

    // only jump if on ground
    if (jump && !on_ground(box.radius, state->get_position()))
        jump = 0;

    jp = jetpack.update(jp);
    cs = this->pack_control_state(forward, backwards, left, right, jp,
                                  jump, crouch, boost, misc1, misc2, misc3);
    return cs;
}

void PlayerAgent::set_control_state(int f, int b, int l, int r, int jet,
                                    int jump, int crouch, int boost, int misc1,
                                    int misc2, int misc3, float theta, float phi)
{
    uint16_t cs = this->pack_control_state(f, b, l, r, jet, jump, crouch,
                                           boost, misc1, misc2, misc3);
    cs = this->sanitize_control_state(cs);
    this->set_control_state(cs, theta, phi);
}

//set actually sends
void PlayerAgent::set_control_state(uint16_t cs, float theta, float phi)
{
    class Agent* a = this->you();
    if (a == NULL) return;  //player agent not set

    cs = this->sanitize_control_state(cs);

    cs_seq_local = (cs_seq_local+1) % 256;

    AgentState s = a->get_state();
    if (a->status.dead)
    {   // use the last agent state's theta,phi (instead of the camera which is normally passed in)
        theta = s.theta;
        phi = s.phi;
    }

    agent_control_state_CtoS csp;
    csp.seq = cs_seq_local;
    csp.cs = cs;
    csp.theta = theta;
    csp.phi = phi;
    csp.send();

    //save control state
    int index = cs_seq_local % 128;

    cs_local[index].seq = cs_seq_local;
    cs_local[index].cs = cs;
    cs_local[index].theta = theta;
    cs_local[index].phi = phi;
    //clear out next control state
    cs_local[(index + 1) % 128].seq = -1;
    //client side tick forward

    struct ControlState acs = cs_local[index];

    int cs_index = (state_history_seq) % 256;
    class AgentState tmp = state_history[state_history_index];

    int stop_index = cs_seq_local;

    while (cs_index != stop_index)
    {
        acs = cs_local[cs_index % 128]; //check seq number
        tmp = agent_tick(acs, a->get_bounding_box(acs), tmp);
        tmp.seq = (tmp.seq + 1) % 256;
        cs_index = (cs_index + 1) % 256;
    }
    s0 = tmp;
    acs = cs_local[cs_seq_local % 128];
    s1 = agent_tick(acs, a->get_bounding_box(acs), s0);

    this->movement_event(s0, s1);
}

float PlayerAgent::camera_height()
{
    class Agent* a = this->you();
    if (a == NULL) return 0.0f;
    return a->camera_height();
}

float PlayerAgent::camera_z()
{
    if (this->you() == NULL) return 0.0f;
    return this->camera_state.z + this->camera_height();
}

struct Vec3 PlayerAgent::camera_position()
{
    if (this->you() == NULL) return vec3_init(0,0,0);
    return vec3_init(this->camera_state.x, this->camera_state.y, this->camera_z());
}

void PlayerAgent::play_geiger()
{
    static int i = 0;
    i++;
    static int skip = 0;

    Agent* a = this->you();
    if (a == NULL) return;
    int rad_level = get_attribute_int(a->id, "rad_level");
    if (!rad_level) return;
    int rad_level_max = get_attribute_int(a->id, "rad_level_max");

    const int slowest_rate = ONE_SECOND;
    int rate = (float(slowest_rate) / float(rad_level_max)) * (rad_level_max - rad_level + 1);
    rate = GS_MAX(rate, 1);

    skip = GS_MAX(0, skip-1);
    if (skip) return;
    if (i % rate == 0)
    {
        int snd_id = Sound::play_2d_sound("geiger");
        if (snd_id >= 0)
        {
            float pmult = float(randrange(-7,7)) / 1000.0f;
            float gmult = float(randrange(-2,2)) / 1000.0f;
            Sound::set_pitch_multiplier(snd_id, pmult + 1.0f);
            Sound::set_gain_multiplier(snd_id, gmult + 1.0f);
        }
        // prevent sounds from playing too close
        skip = randrange(1, 2);
        int variance = rate/2;
        i += rate + randrange(-variance, variance/2);
    }
}

void PlayerAgent::update_sound()
{
    Sound::update_listener(this->camera_state.get_position(),
                           this->camera_state.get_velocity(),
                           agent_camera->forward_vector(),
                           vec3_init(0, 0, 1));
    this->play_geiger();
}

PlayerAgent::PlayerAgent() :
    crouching(false), camera_mode(CAMERA_STATE_CLIENT_SIDE_PREDICTION_INTERPOLATED),
    cs_seq_local(255), cs_seq_net(-1),
    state_history_seq(0), state_history_index(0),
    agent_id(NULL_AGENT), action(this)
{
    state_history = new AgentState[AGENT_STATE_HISTORY_SIZE];
    for (int i=0; i<128; cs_local[i++].seq = -1);
    for (int i=0; i<128; cs_net[i++].seq = -1) ;
}

PlayerAgent::~PlayerAgent()
{
    delete[] this->state_history;
}

void PlayerAgent::toggle_camera_mode()
{
    camera_mode = (camera_mode + 1) % CAMERA_STATE_END;
    switch (camera_mode)
    {
        case CAMERA_STATE_NET_AGENT:
            printf("Camera Mode: net_agent\n");
            break;
        case CAMERA_STATE_CLIENT_SIDE_PREDICTION_INTERPOLATED:
            printf("Camera Mode: client_side_prediction_interpolated\n");
            break;
        case CAMERA_STATE_CLIENT_SIDE_PREDICTION:
            printf("Camera Mode: client_side_prediction\n");
            break;
        case CAMERA_STATE_LAST_SERVER_SNAPSHOT:
            printf("Camera Mode: last_server_snapshot\n");
            break;

        case CAMERA_STATE_END:
            printf("PlayerAgent toggle_camera_mode: error\n");
            break;
    }
}

void PlayerAgent::pump_camera()
{
    switch (this->camera_mode)
    {
        case CAMERA_STATE_NET_AGENT:
            if (this->you() != NULL)
                this->camera_state = this->you()->get_state();
            break;
        case CAMERA_STATE_CLIENT_SIDE_PREDICTION_INTERPOLATED:   // default
            this->update_client_side_prediction_interpolated();
            this->camera_state = c;
            break;
        case CAMERA_STATE_CLIENT_SIDE_PREDICTION:
            this->camera_state = this->s1;
            break;
        case CAMERA_STATE_LAST_SERVER_SNAPSHOT:
            this->camera_state = this->state_snapshot;
            break;

        case CAMERA_STATE_END:
            GS_ASSERT(false);
            break;
    }
}

void PlayerAgent::update_model()
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
    Voxels::VoxDat* vox_dat = &VoxDats::agent;
    float radius = a->vox->get_part(0)->radius;
    if (!sphere_fulstrum_test_translate(center.x, center.y, center.z, radius))
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

bool PlayerAgent::facing_block(int pos[3])
{
    if (this->you() == NULL) return NULL;
    if (current_camera == NULL) return NULL;

    const float max_distance = 4.0f;

    class RaytraceData data;
    bool collided = raytrace_terrain(current_camera_position, current_camera->forward_vector(), max_distance, &data);
    if (!collided) return false;
    for (int i=0; i<3; i++)
        pos[i] = data.collision_point[i];
    return collided;
}

int PlayerAgent::facing_container()
{
    int pos[3];
    bool facing = this->facing_block(pos);
    if (!facing) return NULL_CONTAINER;

    return t_map::get_block_item_container(pos[0], pos[1], pos[2]);
}

bool PlayerAgent::nearest_open_block(const float max_dist, struct Vec3i& pos)
{
    if (agent_camera == NULL) return NULL;
    struct Vec3 f = agent_camera->forward_vector();
    class RaytraceData data;
    bool collided = raytrace_terrain(agent_camera->get_position(), f, max_dist, &data);
    if (!collided) return false;
    data.get_pre_collision_point(pos);
    return collided;
}

int PlayerAgent::get_facing_side(int solid_pos[3], int open_pos[3], int side[3], float* distance)
{
    if (agent_camera == NULL) return 0;
    struct Vec3 p = agent_camera->get_position();
    struct Vec3 v = agent_camera->forward_vector();

    const float max_dist = 128.0f;
    class RaytraceData data;
    bool collided = raytrace_terrain(p, v, max_dist, &data);
    if (!collided) return 0;

    for (int i=0; i<3; i++) solid_pos[i] = data.collision_point[i];
    data.get_pre_collision_point(open_pos);
    data.get_side_array(side);
    *distance = data.interval * max_dist;

    GS_ASSERT(open_pos[2] >= 0); // agent should never be shooting from underground
    return data.get_cube_type();
}

int PlayerAgent::get_facing_side(int solid_pos[3], int open_pos[3], float* distance)
{
    int s[3];
    int block = this->get_facing_side(solid_pos, open_pos, s, distance);
    if (block <= 0)
        return -1;
    return get_cube_side_from_side_array(s);
}

void PlayerAgent::fell(float dvz)
{   // dvz is the change in vertical velocity
    // TODO -- multiply dvz by base gain to get playable gain for fall range
    static const float SOFT_FALL_VELOCITY_THRESHOLD   = 0.07f;
    static const float MEDIUM_FALL_VELOCITY_THRESHOLD = 0.50f;
    static const float HARD_FALL_VELOCITY_THRESHOLD   = 0.75f;
    if (dvz > HARD_FALL_VELOCITY_THRESHOLD)
        Sound::play_2d_sound("hard_fall");
    else
    if (dvz > MEDIUM_FALL_VELOCITY_THRESHOLD)
        Sound::play_2d_sound("medium_fall");
    else
    if (dvz > SOFT_FALL_VELOCITY_THRESHOLD)
        Sound::play_2d_sound("soft_fall");
}

void PlayerAgent::movement_event(const AgentState& s0, const AgentState& s1)
{
    // TODO: detect if player is on ground
    // TODO: detect when player goes form on-ground to ground (fall event)
    // TODO: detect the type of block the player is walking on
    // TODO: random plus-minue variation in footstep
    Agent* a = this->you();
    if (a == NULL) return;
    static const float distance_per_step = 1.5f;
    static float total_distance = 0.0f;
    BoundingBox box = a->get_bounding_box();
    bool s1_on_ground = on_ground(box.radius, s1.get_position());
    bool camera_on_ground = on_ground(box.radius, this->camera_state.get_position());
    float dx = s1.x - quadrant_translate_f(s1.x, s0.x);
    float dy = s1.y - quadrant_translate_f(s1.y, s0.y);
    float d = sqrtf(dx*dx + dy*dy);
    total_distance += d;
    float dvz = s1.vz - s0.vz;
    float dz  = s1.z  - s0.z;
    //if (dvz) printf("z: %f, vz: %f, dz: %f, dvz: %f. on_ground? %d\n", s1.z, s1.vz, dz, dvz, s1_on_ground);
    if (dz < 0 && s1_on_ground) // was falling & hit ground
        this->fell(dvz);

    if (!camera_on_ground) return;

    // pre-programmed footstep sequence
    const size_t n_footsteps = 16;
    int footsteps[n_footsteps] =
    {
        2,
        1,
        2,
        1,
        3,
        1,
        2,
        4,

        2,
        1,
        2,
        2,
        3,
        1,
        4,
        2,
    };

    const size_t n_perturb_footsteps = 4;
    int perturb_footsteps[n_perturb_footsteps] = {1,2,3,4};

    #define RANDOM_STEPS 0
    #define RANDOM_CHANGE 1
    #define RANDOM_CHANGE_PER_LOOP 1

    const size_t footstep_sound_len = 32;
    static char footstep_sound[footstep_sound_len] = {'\0'};
    if (total_distance > distance_per_step)
    {
        total_distance = fmodf(total_distance, distance_per_step);
        #if RANDOM_STEPS
        int footstep_num = footsteps[randrange(0, n_footsteps-1)];
        #else
        int footstep_num = 1;
        static size_t step = 0;
        step++;
        step %= n_footsteps;
        if (RANDOM_CHANGE && randf() < (((float)RANDOM_CHANGE_PER_LOOP)/((float)n_footsteps)))
            footstep_num = perturb_footsteps[randrange(0,n_perturb_footsteps-1)];
        else
            footstep_num = footsteps[step];
        #endif

        size_t wrote = snprintf(footstep_sound, footstep_sound_len, "soft_step_%d", footstep_num);
        IF_ASSERT(wrote >= footstep_sound_len) return;
        Sound::play_2d_sound(footstep_sound);
    }

    #undef RANDOM_STEPS
    #undef RANDOM_CHANGE
    #undef RANDOM_CHANGE_PER_LOOP
}


#if !PRODUCTION
void PlayerAgent::teleport_to(struct Vec3 p)
{
    teleport_me_CtoS msg;
    msg.position = p;
    msg.send();
}
#endif

}   // Agents
