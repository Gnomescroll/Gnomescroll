#include "agent.hpp"

#include <agent/net_agent.hpp>
#include <agent/agent_physics.hpp>
#include <common/defines.h>
#include <math.h>
#include <common/random.hpp>
#include <item/toolbelt/_interface.hpp>
#include <item/common/constants.hpp>
#include <physics/quadrant.hpp>
#if DC_SERVER
#include <t_gen/explosives.hpp>
#endif
#if DC_CLIENT
# include <common/compat_gl.h>
# include <agent/client/agent_draw.hpp>
# include <agent/client/player_agent.hpp>
#endif

namespace Agents
{

struct Vec3 AgentState::forward_vector()
{
    IF_ASSERT(theta > 1.0f)
        theta -= 2.0f;
    IF_ASSERT(theta < -1.0f)
        theta += 2.0f;

    IF_ASSERT(phi > 0.4999f)
        phi = 0.4999f;
    IF_ASSERT(phi < -0.4999f)
        phi = -0.4999f;

    Vec3 f = vec3_init(1.0f, 0.0f, 0.0f);
    f = vec3_euler_rotation(f, theta, phi, 0.0f);
    normalize_vector(&f);

    return f;
}

void Agent::teleport(float x,float y,float z)
{
    this->set_position(x,y,z);

    #if DC_SERVER
    Agent_teleport_message msg;
    msg.id = id;
    msg.x = s.x;
    msg.y = s.y;
    msg.z = s.z;
    msg.vx = s.vx;
    msg.vy = s.vy;
    msg.vz = s.vz;
    msg.theta = s.theta;
    msg.phi = s.phi;
    msg.broadcast();

    t_map::t_map_manager_update_client_position(this->client_id, x,y);
    #endif
}

void Agent::teleport(float x,float y,float z, float vx, float vy, float vz, float theta, float phi)
{
    this->set_state(x,y,z,vx,vy,vz);
    s.theta = theta;
    s.phi = phi;

    #if DC_SERVER
    Agent_teleport_message msg;
    msg.id = id;
    msg.x = s.x;
    msg.y = s.y;
    msg.z = s.z;
    msg.vx = s.vx;
    msg.vy = s.vy;
    msg.vz = s.vz;
    msg.theta = s.theta;
    msg.phi = s.phi;
    msg.broadcast();

    t_map::t_map_manager_update_client_position(this->client_id, x,y);
    #endif
}

#if DC_SERVER
void Agent::tick()
{
    AgentState tmp = this->s;
    int start_seq = this->cs_seq;
    while (this->cs[this->cs_seq].seq == this->cs_seq)
    {
        tmp = agent_tick(this->cs[this->cs_seq], this->box, tmp);
        this->cs_seq = (this->cs_seq + 1) % 256;
    }
    if (start_seq != this->cs_seq)
        this->movement_event(s, tmp);
    this->s = tmp;
    const float Z_DEATH_ZONE = -200.0f;
    if (!this->status.dead && this->s.z < Z_DEATH_ZONE)
        this->status.die(this->id, OBJECT_AGENT, DEATH_BELOW_MAP);
    else
        this->status.respawn();
    this->status.tick();
}
#endif

#if DC_CLIENT
void Agent::tick()
{
    while (this->cs[this->cs_seq].seq == this->cs_seq)
    {
        this->s = agent_tick(this->cs[this->cs_seq], this->box, this->s);
        this->cs_seq = (this->cs_seq + 1) % 256;
    }
}
#endif


void Agent::handle_control_state(int seq, int cs, float theta, float phi)
{
    IF_ASSERT(seq < 0 || seq >= 256) return;
    this->cs[seq].seq = seq;
    this->cs[seq].cs = cs;
    this->cs[seq].theta = theta;
    this->cs[seq].phi = phi;

    this->tick();

    #if DC_SERVER
    class PlayerAgent_Snapshot p;
    p.id = id;
    p.seq = cs_seq;
    p.x = s.x;
    p.y = s.y;
    p.z = s.z;
    p.vx = s.vx;
    p.vy = s.vy;
    p.vz = s.vz;
    p.theta = s.theta;
    p.phi = s.phi;
    p.sendToClient(client_id);

    if (seq % 32 == 0)
    {
        class Agent_state_message a;
        a.seq = cs_seq;
        a.x = s.x;
        a.y = s.y;
        a.z = s.z;
        a.vx = s.vx;
        a.vy = s.vy;
        a.vz = s.vz;
        a.theta = s.theta;
        a.phi = s.phi;
        a.id = this->id;
        a.broadcast();

        //clean out old control state
        for (int i=16; i<96; i++)
        {
            int index = (seq + i)%256;
            this->cs[index].seq = -1;
        }
    }
    #endif
}

#if DC_CLIENT
bool Agent::is_you()
{
    return (this->id != NULL_AGENT && this->id == ClientState::player_agent.agent_id);
}

void Agent::handle_state_snapshot(int seq, float theta, float phi,
                                  float x, float y, float z,
                                  float vx, float vy, float vz)
{
    this->state_snapshot.seq = seq;
    this->state_snapshot.theta = theta;
    this->state_snapshot.phi = phi;
    this->state_snapshot.x = x;
    this->state_snapshot.y = y;
    this->state_snapshot.z = z;
    this->state_snapshot.vx = vx;
    this->state_snapshot.vy = vy;
    this->state_snapshot.vz = vz;
    for (int i=16; i<96; i++)
    {
        int index = (seq + i) % 256;
        this->cs[index].seq = -1;
    }
    this->state_rollback = this->state_snapshot; //when new snapshot comes, in, set rollbacks
    this->cs_seq = seq;
    this->s = this->state_snapshot;
    this->tick();
}
#endif

void Agent::set_position(float x, float y, float z)
{
    this->s.x = translate_point(x);
    this->s.y = translate_point(y);
    this->s.z = z;
}

void Agent::set_state(float  x, float y, float z, float vx, float vy, float vz)
{
    this->set_position(x,y,z);
    this->s.vx = vx;
    this->s.vy = vy;
    this->s.vz = vz;
}

void Agent::set_state_snapshot(float  x, float y, float z, float vx, float vy, float vz)
{
    this->state_snapshot.x = translate_point(x);
    this->state_snapshot.y = translate_point(y);
    this->state_snapshot.z = z;
    this->state_snapshot.vx = vx;
    this->state_snapshot.vy = vy;
    this->state_snapshot.vz = vz;
}

void Agent::set_angles(float theta, float phi)
{
    this->s.theta = theta;
    this->s.phi = phi;
}

#if DC_SERVER
void Agent::fell(float vz)
{
    static const float health_max = get_base_attribute_int("health_max");
    static const float FALL_DAMAGE_PER_METER = health_max/(FALL_DEATH_HEIGHT-FALL_SAFE_HEIGHT);
    static const float ig = -1.0f / (2.0f * AGENT_GRAVITY);
    float fall_distance = (vz*vz) * ig;
    fall_distance -= FALL_SAFE_HEIGHT;
    float damage = roundf(FALL_DAMAGE_PER_METER * fall_distance);
    if (damage <= 0) return;
    this->status.apply_damage(damage, DEATH_FALL);
}

void Agent::movement_event(const AgentState& s0, const AgentState& s1)
{
    bool s1_on_ground = on_ground(this->box.box_r, s1.x, s1.y, s1.z);
    float dz  = s1.z - s0.z;
    if (dz < 0 && s1_on_ground) // was falling & hit ground
        this->fell(s0.vz);
}

void Agent::set_camera_state(float x, float y, float z, float theta, float phi)
{
    this->camera.x = translate_point(x);
    this->camera.y = translate_point(y);
    this->camera.z = z;
    this->camera.theta = theta;
    this->camera.phi = phi;
}

void Agent::get_spawn_point(struct Vec3* spawn)
{
    struct Vec3 default_spawn = vec3_init(map_dim.x/2, map_dim.y/2, map_dim.z-1);
    float fh = this->current_height();
    if (this->status.spawner == BASE_SPAWN_ID)
    {
        Entities::Entity* base = Entities::get(OBJECT_BASE, 0);
        IF_ASSERT(base == NULL)
        {
            *spawn = default_spawn;
            return;
        }
        using Components::AgentSpawnerComponent;
        AgentSpawnerComponent* spawner = (AgentSpawnerComponent*)base->get_component(COMPONENT_AGENT_SPAWNER);
        IF_ASSERT(spawner == NULL) *spawn = default_spawn;
        else *spawn = spawner->get_spawn_point(fh, this->box.box_r);
    }
    else
    {   // spawner was found
        Entities::Entity* spawner = Entities::get(OBJECT_AGENT_SPAWNER, this->status.spawner);
        IF_ASSERT(spawner == NULL)
            *spawn = default_spawn;
        else
        {
            using Components::AgentSpawnerComponent;
            AgentSpawnerComponent* spawner_comp = (AgentSpawnerComponent*)spawner->get_component(COMPONENT_AGENT_SPAWNER);
            IF_ASSERT(spawner_comp == NULL)
                *spawn = default_spawn;
            else
                *spawn = spawner_comp->get_spawn_point(fh, this->box.box_r);
        }
    }
}

void Agent::spawn_state()
{   // update position
    struct Vec3 spawn;
    this->get_spawn_point(&spawn);
    this->spawn_state(spawn);
}

void Agent::spawn_state(struct Vec3 p)
{   // update position
    float theta = this->status.get_spawn_angle();
    this->teleport(p.x, p.y, p.z, 0, 0, 0, theta, 0.0f);
}
#endif

void Agent::init_vox()
{
    this->vox = new Voxels::VoxelModel(&VoxDats::agent, this->id, this->type);
    this->vox->set_hitscan(true);
    this->vox->register_hitscan();
}

Agent::Agent(AgentID id) :
    id(id), client_id((ClientID)id), user_id(NULL_USER_ID),
    type(OBJECT_AGENT), status(this)
    #if DC_CLIENT
    , event(this)
    , initial_teleport(false)
    #endif
    #if DC_SERVER
    , camera_ready(false)
    #endif
{
    box.height = AGENT_HEIGHT;
    box.crouch_height = AGENT_HEIGHT_CROUCHED;
    box.box_r = AGENT_BOX_RADIUS;

    cs_seq = 0;

    printf("Created agent: %d\n", id);

    state_snapshot.seq = -1;
    state_rollback.seq = -1;
    for (int i=0; i<256; i++)
    {
        cs[i].seq = -1;
        cs[i].cs = 0;
        cs[i].theta = 0;
        cs[i].phi = 0;
    }

    set_angles(0.5f, 0.0f);
    set_position(0, 0, -256.0f);
}

Agent::~Agent()
{
    delete this->vox;
}


void Agent::revert_to_snapshot()
{
    this->s = state_snapshot;
    this->cs_seq = state_snapshot.seq;
}

void Agent::revert_to_rollback()
{
    this->s = state_rollback;
    this->cs_seq = state_rollback.seq;
}

void Agent::print_cs()
{
    uint16_t cs = this->cs[this->cs_seq].cs;
    int forward   = cs & CS_FORWARD  ? 1 : 0;
    int backwards = cs & CS_BACKWARD ? 1 : 0;
    int left      = cs & CS_LEFT     ? 1 : 0;
    int right     = cs & CS_RIGHT    ? 1 : 0;
    int jetpack   = cs & CS_JETPACK  ? 1 : 0;
    int jump      = cs & CS_JUMP     ? 1 : 0;
    int crouch    = cs & CS_CROUCH   ? 1 : 0;
    int boost     = cs & CS_BOOST    ? 1 : 0;
    int misc1     = cs & CS_MISC1    ? 1 : 0;
    int misc2     = cs & CS_MISC2    ? 1 : 0;
    int misc3     = cs & CS_MISC3    ? 1 : 0;

    printf("f,b,l,r = %d%d%d%d\n", forward, backwards, left, right);
    printf("jet=%d\n", jetpack);
    printf("jump=%d\n", jump);
    printf("crouch=%d\n", crouch);
    printf("boost=%d\n", boost);
    printf("misc123= %d%d%d\n", misc1, misc2, misc3);
}

AgentControlState Agent::get_current_control_state()
{
    return this->cs[(this->cs_seq - 1 + 256) % 256];
}

bool Agent::crouched()
{
    return (this->get_current_control_state().cs & CS_CROUCH);
}

float Agent::camera_height()
{
    if (this->status.dead)
        return CAMERA_HEIGHT_DEAD;
    else if (this->crouched())
        return CAMERA_HEIGHT_CROUCHED;
    return CAMERA_HEIGHT;
}

float Agent::current_height()
{
    if (this->crouched())
        return this->box.crouch_height;
    else
        return this->box.height;
}

int Agent::current_height_int()
{
    float h = this->current_height();
    return int(ceilf(h));
}

float Agent::camera_z()
{
    return this->s.z + this->camera_height();
}

struct Vec3 Agent::camera_position()
{
    return vec3_init(this->s.x, this->s.y, this->camera_z());
}

class Voxels::VoxelVolume* Agent::get_arm()
{
    if (this->vox == NULL) return NULL;
    return this->vox->get_part(AGENT_PART_RARM);
}

struct Vec3 Agent::arm_center()
{
    if (this->vox == NULL || !this->vox->was_updated)
        return this->get_center();
    return this->vox->get_part(AGENT_PART_RARM)->get_center();
}

struct Vec3 Agent::arm_forward()
{
    if (this->vox == NULL) return vec3_init(1,0,0);
    return this->vox->get_part(AGENT_PART_RARM)->world_matrix.vx;
}

struct Vec3 Agent::arm_right()
{
    if (this->vox == NULL) return vec3_init(0,1,0);
    return this->vox->get_part(AGENT_PART_RARM)->world_matrix.vy;
}

struct Vec3 Agent::arm_up()
{
    if (this->vox == NULL) return vec3_init(0,0,1);
    return this->vox->get_part(AGENT_PART_RARM)->world_matrix.vz;
}

int Agent::get_facing_block_type()
{
    const float CUBE_SELECT_MAX_DISTANCE = 12.0f;
    struct Vec3 f;
    struct Vec3 p;

    #if DC_CLIENT
    if (this->is_you())
    {   // if this is your agent, use camera / player agent state instead.
        p = agent_camera->get_position();
        f = agent_camera->forward_vector();
    }
    else
    #endif
    {
        p = this->camera_position();
        f = this->s.forward_vector();
    }

    class RaytraceData data;
    bool collided = raytrace_terrain(p, f, CUBE_SELECT_MAX_DISTANCE, &data);

    if (!collided) return 0;
    return data.get_cube_type();
}

bool Agent::point_can_cast(float x, float y, float z, float max_dist)
{
    // checks if a point can raycast to some area of the agent box,
    // or if the terrain prevents it

    // sample points on the outside of the agent box
    const int samples_per_height = 8;
    float step = this->current_height() / samples_per_height;

    // check the center column
    struct Vec3 start = vec3_init(x,y,z);
    struct Vec3 end = this->s.get_position();
    for (int i=0; i<samples_per_height; i++)
    {
        end.z = this->s.z + i*step;
        if (vec3_distance(start, end) < max_dist
         && raytrace_terrain(start, end))
            return true;
    }

    // check the 4 corner columns
    end.x = this->s.x + this->box.box_r;
    end.y = this->s.y + this->box.box_r;
    for (int i=0; i<samples_per_height; i++)
    {
        end.z = this->s.z + i*step;
        if (vec3_distance(start, end) < max_dist
         && !raytrace_terrain(start, end))
            return true;
    }

    end.x = this->s.x + this->box.box_r;
    end.y = this->s.y - this->box.box_r;
    for (int i=0; i<samples_per_height; i++)
    {
        end.z = this->s.z + i*step;
        if (vec3_distance(start, end) < max_dist
         && !raytrace_terrain(start, end))
            return true;
    }

    end.x = this->s.x - this->box.box_r;
    end.y = this->s.y + this->box.box_r;
    for (int i=0; i<samples_per_height; i++)
    {
        end.z = this->s.z + i*step;
        if (vec3_distance(start, end) < max_dist
         && !raytrace_terrain(start, end))
            return true;
    }

    end.x = this->s.x - this->box.box_r;
    end.y = this->s.y - this->box.box_r;
    for (int i=0; i<samples_per_height; i++)
    {
        end.z = this->s.z + i*step;
        if (vec3_distance(start, end) < max_dist
         && !raytrace_terrain(start, end))
            return true;
    }

    return false;
}

bool Agent::in_sight_of(Vec3 source, Vec3* sink)
{
    if (this->vox == NULL) return false;
    return this->vox->in_sight_of(source, sink);
}

bool Agent::in_sight_of(Vec3 source, Vec3* sink, float failure_rate)
{
    if (this->vox == NULL) return false;
    return this->vox->in_sight_of(source, sink, failure_rate);
}

void Agent::update_legs()
{
    const float arc = 25.0f;    // degree
    const int peak = 37;
    const int rest = (peak-1)/2;
    const float m = (arc/180)/((float)rest);

    static int idle = 0;
    static int legtick = 0;
    static float direction = -1;

    AgentControlState cs = this->get_current_control_state();
    bool forward = (cs.cs & CS_FORWARD) != 0;
    bool backward = (cs.cs & CS_BACKWARD) != 0;
    bool left = (cs.cs & CS_LEFT) != 0;
    bool right = (cs.cs & CS_RIGHT) != 0;

    static bool was_forward = false;

    if (forward || backward || left || right)
    {
        if (was_forward && backward && !forward)
            direction *= -1;
        if (!was_forward && forward && !backward)
            direction *= -1;
        if (legtick == peak || legtick == 0)
            direction *= -1;
        if (direction < 0)
            legtick--;
        else
            legtick++;
        int swing = (legtick%peak) - rest;
        this->vox->set_node_rotation_by_part(AGENT_PART_RLEG, 0, m*swing, 0);
        this->vox->set_node_rotation_by_part(AGENT_PART_LLEG, 0, -m*swing, 0);
    }
    else
    {
        idle++;
        if (idle >= (legtick%peak)-rest)
        {
            legtick = rest;
            idle = 0;
            this->vox->set_node_rotation_by_part(AGENT_PART_RLEG, 0, 0, 0);
            this->vox->set_node_rotation_by_part(AGENT_PART_LLEG, 0, 0, 0);
        }
        else
        {
            int swing = (legtick%peak) - (idle+rest);
            this->vox->set_node_rotation_by_part(AGENT_PART_RLEG, 0, m*swing, 0);
            this->vox->set_node_rotation_by_part(AGENT_PART_LLEG, 0, -m*swing, 0);
        }
    }

    if (forward)
        was_forward = true;
    if (backward)
        was_forward = false;
}

void Agent::update_model()
{
    #if DC_CLIENT
    if (this->vox == NULL) return;

    if (this->event.color_changed)
    {    // set color of model
        this->vox->fill_color(this->status.color);
        this->event.color_changed = false; // reset
    }

    if (this->is_you())
    {   // your agent
        ClientState::player_agent.update_model();
        return;
    }

    Vec3 center = this->get_center();    // model is fresh, use model center for more accurate culling

    this->vox->was_updated = false;
    // other agents
    Voxels::VoxDat* vox_dat = &VoxDats::agent;
    float radius = this->vox->get_part(0)->radius;

    if (!sphere_fulstrum_test_translate(center.x, center.y, center.z, radius))
    {   // agent not in view fulcrum
        this->vox->set_draw(false);
        this->vox->set_hitscan(false);
        return;
    }

    if (this->crouched())
    {
        vox_dat = &VoxDats::agent_crouched;
        if (!this->status.vox_crouched)
        {
            this->vox->set_vox_dat(vox_dat);
            this->vox->reset_skeleton();
            this->status.vox_crouched = true;
        }
    }
    else
    {
        if (this->status.vox_crouched)
        {   // was crouched last frame, but not this frame: restore standing model
            this->vox->set_vox_dat(vox_dat);
            this->vox->reset_skeleton();
            this->status.vox_crouched = false;
        }
    }
    if (this->status.dead) vox_dat = &VoxDats::agent_dead;

    this->vox->set_vox_dat(vox_dat);
    this->update_legs();
    this->vox->update(this->s.x, this->s.y, this->s.z, this->s.theta, -this->s.phi);
    this->vox->set_draw(true);
    this->vox->set_hitscan(true);
    #endif

    #if DC_SERVER
    if (this->vox == NULL) return;
    this->vox->was_updated = false;
    Voxels::VoxDat* vox_dat = &VoxDats::agent;
    if (this->crouched())
    {
        vox_dat = &VoxDats::agent_crouched;
        if (!this->status.vox_crouched)
        {
            this->vox->set_vox_dat(vox_dat);
            this->vox->reset_skeleton();
            this->status.vox_crouched = true;
        }
    }
    else
    {
        if (this->status.vox_crouched)
        {   // was crouched last frame, but not this frame: restore standing model
            this->vox->set_vox_dat(vox_dat);
            this->vox->reset_skeleton();
            this->status.vox_crouched = false;
        }
    }
    if (this->status.dead) vox_dat = &VoxDats::agent_dead;

    this->vox->set_vox_dat(vox_dat);
    this->update_legs();
    this->vox->update(this->s.x, this->s.y, this->s.z, this->s.theta, -this->s.phi);
    this->vox->set_hitscan(true);
    #endif
}

bool Agent::near_base()
{
    Entities::Entity* b = Entities::get(OBJECT_BASE, 0);
    if (b == NULL) return false;
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)b->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return false;
    Vec3 bp = physics->get_position();

    float x = quadrant_translate_f(this->s.x, bp.x);
    float y = quadrant_translate_f(this->s.y, bp.y);
    float z = bp.z;
    using Entities::BASE_SPAWN_RADIUS;
    static const float k = sqrtf(2);    // scaling factor for radius of a square (base spawning is square manhattan distance)
    if (distancef_squared(x,y,z, this->s.x, this->s.y, this->s.z) < k*BASE_SPAWN_RADIUS*BASE_SPAWN_RADIUS)
        return true;
    return false;
}

bool Agent::nearest_open_block(const float max_dist, int open_point[3])
{
    Vec3 p = this->get_camera_position();
    Vec3 f = this->forward_vector();
    class RaytraceData data;
    bool collided = raytrace_terrain(p, f, max_dist, &data);
    if (!collided) return false;
    data.get_pre_collision_point(open_point);
    return collided;
}

void force_update_agent_vox(Agent* a)
{
    IF_ASSERT(a == NULL) return;
    a->vox->was_updated = false;
    AgentState s = a->get_state();
    a->vox->update(s.x, s.y, s.z, s.theta, -s.phi);
}

// returns block type
int Agent::get_facing_side(int solid_pos[3], int open_pos[3], int side[3], float* distance)
{
    Vec3 p = this->get_camera_position();
    Vec3 v = this->forward_vector();

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

// returns side as int
int Agent::get_facing_side(int solid_pos[3], int open_pos[3], float* distance)
{
    int s[3];
    int block = this->get_facing_side(solid_pos, open_pos, s, distance);
    if (block <= 0) return -1;
    return get_cube_side_from_side_array(s);
}

// returns side as int
int Agent::get_facing_side(int solid_pos[3], int open_pos[3], const float max_distance)
{
    Vec3 p = this->get_camera_position();
    Vec3 v = this->forward_vector();

    class RaytraceData data;
    bool collided = raytrace_terrain(p, v, max_distance, &data);
    if (!collided) return -1;

    data.get_pre_collision_point(open_pos);
    for (int i=0; i<3; i++)
        solid_pos[i] = data.collision_point[i];

    GS_ASSERT(open_pos[2] >= 0); // agent should never be shooting from underground
    GS_ASSERT(solid_pos[2] >= 0); // agent should never be shooting from underground

    return data.side;
}

}   // Agents
