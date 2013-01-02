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
#endif

#if DC_CLIENT
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

#if DC_CLIENT
bool Agent::is_you()
{
    return (this->id != NULL_AGENT && this->id == ClientState::player_agent.agent_id);
}
#endif

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

void Agent::tick()
{
    int _tc =0;
    struct AgentControlState _cs;

    while (cs[CS_seq].seq == CS_seq)
    {
        _cs = cs[CS_seq];
        s = _agent_tick(_cs, box, s);

        CS_seq = (CS_seq+1)%256;
        _tc++;
    }

    #if DC_SERVER
    const float Z_DEATH_ZONE = -200.0f;
    if (!this->status.dead && this->s.z < Z_DEATH_ZONE)
        this->status.die(this->id, OBJECT_AGENT, DEATH_BELOW_MAP);
    else
        this->status.respawn();
    this->status.tick();
    #endif
}


#define ADVANCED_JUMP 0

//class AgentState _agent_tick(const struct AgentControlState& _cs, const struct AgentCollisionBox& box, const class AgentState& as)
//{
    //int a_cs = _cs.cs;
    ////set control state variables
    //bool forward     = a_cs & CS_FORWARD ? 1 :0;
    //bool backwards   = a_cs & CS_BACKWARD ? 1 :0;
    //bool left        = a_cs & CS_LEFT ? 1 :0;
    //bool right       = a_cs & CS_RIGHT ? 1 :0;
    //bool jetpack     = a_cs & CS_JETPACK ? 1 :0;
    //bool jump        = a_cs & CS_JUMP ? 1 :0;
    //bool crouch      = a_cs & CS_CROUCH ? 1 :0;
    ////implemented, but unused
    ///*
    //bool boost       = a_cs & CS_BOOST ? 1 :0;
    //bool misc1       = a_cs & CS_MISC1 ? 1 :0;
    //bool misc2       = a_cs & CS_MISC2 ? 1 :0;
    //bool misc3       = a_cs & CS_MISC3 ? 1 :0;
    //*/

    //const float pi = 3.14159265f;
    //const float tr = 1.0f / 10.0f;    //tick rate
    //const float tr2 = tr*tr;
    //const float AGENT_MASS = 1.0f; //the agents mass, will become a variable dependent on the amount of stuff a player carries
    //float imass = 1.0f/AGENT_MASS;   // inverse mass
    //const float FRICTION = 0.9f;    // coefficient of friction. will be variable based on the surface
    //float AGENT_FORCE = 2.0f;


    //float z_gravity = -9.8f * tr2;
    //#if DC_CLIENT
    //if (!t_map::position_is_loaded(as.x, as.y)) z_gravity = 0.0f;
    //#endif
    //const struct Vec3 gravity = vec3_init(0.0f, 0.0f, z_gravity * AGENT_MASS);

    //float fx = 0.0f;
    //float fy = 0.0f;
    //if (forward)
    //{
        //fx += cosf( _cs.theta * pi);
        //fy += sinf( _cs.theta * pi);
    //}
    //if (backwards)
    //{
        //fx += -cosf( _cs.theta * pi);
        //fy += -sinf( _cs.theta * pi);
    //}
    //if (left)
    //{
        //fx += cosf( _cs.theta * pi + pi/2);
        //fy += sinf( _cs.theta * pi + pi/2);
    //}
    //if (right)
    //{
        //fx += -cosf( _cs.theta * pi + pi/2);
        //fy += -sinf( _cs.theta * pi + pi/2);
    //}

    //const float precision = 0.000001f;
    //// normalize diagonal motion
    //if (fx < -precision || fx > precision || fy < -precision || fy > precision)
    //{
        //float len = 1.0f/sqrtf(fx*fx + fy*fy);
        //fx *= len;
        //fy *= len;
    //}


    ////const struct Vec3 f = as.forward_vector();
    ////const struct Vec3 f = vec3_init_from_angles(_cs.theta, _cs.phi, 0.0f);
    //const struct Vec3 p = as.get_position();
    //const struct Vec3 v = as.get_velocity();

    //struct Vec3 traction = vec3_scalar_mult(vec3_init(fx, fy, 0.0f), AGENT_FORCE);
    ////struct Vec3 traction = vec3_scalar_mult(f, AGENT_FORCE);
    //struct Vec3 friction = vec3_scalar_mult(v, -FRICTION);
    //struct Vec3 force = vec3_add(gravity, vec3_add(traction, friction));

    //struct Vec3 accel = vec3_scalar_mult(force, imass);

    //struct Vec3 velo = vec3_add(v, vec3_scalar_mult(accel, tr));
    //struct Vec3 pos = vec3_add(p, vec3_scalar_mult(velo, tr));

    //class AgentState _as;
    //_as.set_position(pos);
    //_as.set_velocity(velo);

    //return _as;
//}

//takes an agent state and control state and returns new agent state
class AgentState _agent_tick(const struct AgentControlState& _cs, const struct AgentCollisionBox& box, class AgentState as)
{
    int a_cs = _cs.cs;
    //set control state variables
    bool forward     = a_cs & CS_FORWARD ? 1 :0;
    bool backwards   = a_cs & CS_BACKWARD ? 1 :0;
    bool left        = a_cs & CS_LEFT ? 1 :0;
    bool right       = a_cs & CS_RIGHT ? 1 :0;
    bool jetpack     = a_cs & CS_JETPACK ? 1 :0;
    bool jump        = a_cs & CS_JUMP ? 1 :0;
    bool crouch      = a_cs & CS_CROUCH ? 1 :0;
    //implemented, but unused
    /*
    bool boost       = a_cs & CS_BOOST ? 1 :0;
    bool misc1       = a_cs & CS_MISC1 ? 1 :0;
    bool misc2       = a_cs & CS_MISC2 ? 1 :0;
    bool misc3       = a_cs & CS_MISC3 ? 1 :0;
    */
    const float tr = 1.0f / 10.0f;    //tick rate
    const float tr2 = tr*tr;
    float speed = AGENT_SPEED * tr;
    float height = box.b_height;
    if (crouch)
    {
        speed = AGENT_SPEED_CROUCHED * tr;
        height = box.c_height;
    }

    float z_gravity = -3.0f * tr2;
    #if DC_CLIENT
    if (!t_map::position_is_loaded(as.x, as.y)) z_gravity = 0.0f;
    #endif

    const float z_jetpack = -z_gravity + tr2;

    #if ADVANCED_JUMP
    const float JUMP_POWINITIAL = 1.0f * 0.17f;
    const float JUMP_POWDEC = 0.2f * 0.24f;
    #else
    const float JUMP_POW = AGENT_JUMP_POWER;
    #endif
    //const float z_bounce = 0.10f;
    //const float z_bounce_v_threshold = 1.5f * tr;

    const float pi = 3.14159265f;
    float solid_z = (float)t_map::get_solid_block_below(as.x, as.y, as.z);
    float dist_from_ground = as.z - solid_z;

    float new_x = as.x + as.vx;
    float new_y = as.y + as.vy;
    float new_z = as.z + as.vz;

    //collision
    bool current_collision = collision_check_final_current(box.box_r, height, as.x,as.y,as.z);
    if (current_collision)
    {
        as.x = new_x;
        as.y = new_y;
        as.z += 0.02f; //nudge factor
        if (as.vz < 0.0f) as.vz = 0.0f;
        return as;
    }

    // Collision Order: as.x,as.y,as.z
    bool collision_x = collision_check_final_xy(box.box_r, height, new_x,as.y,as.z);
    if (collision_x)
    {
        new_x = as.x;
        as.vx = 0.0f;
    }

    bool collision_y = collision_check_final_xy(box.box_r, height, new_x,new_y,as.z);
    if (collision_y)
    {
        new_y = as.y;
        as.vy = 0.0f;
    }

    //top and bottom matter
    bool top = false;
    bool collision_z = collision_check_final_z(box.box_r, height, new_x, new_y, new_z, &top);
    if (collision_z)
    {
        new_z = (int)as.z;  // clamp to the floor
        if (top)
            new_z = (float)floor(as.z) + (float)ceil(height) - height;
        as.vz = 0.0f;
    }

    #if ADVANCED_JUMP
    as.jump_pow = new_jump_pow;
    #endif

    float CS_vx = 0.0f;
    float CS_vy = 0.0f;

    if (forward)
    {
        CS_vx += speed*cosf( _cs.theta * pi);
        CS_vy += speed*sinf( _cs.theta * pi);
    }
    if (backwards)
    {
        CS_vx += -speed*cosf( _cs.theta * pi);
        CS_vy += -speed*sinf( _cs.theta * pi);
    }
    if (left)
    {
        CS_vx += speed*cosf( _cs.theta * pi + pi/2);
        CS_vy += speed*sinf( _cs.theta * pi + pi/2);
    }
    if (right)
    {
        CS_vx += -speed*cosf( _cs.theta * pi + pi/2);
        CS_vy += -speed*sinf( _cs.theta * pi + pi/2);
    }

    const float precision = 0.000001f;
    // normalize diagonal motion
    if (CS_vx < -precision || CS_vx > precision || CS_vy < -precision || CS_vy > precision)
    {
        float len = 1.0f/sqrtf(CS_vx*CS_vx + CS_vy*CS_vy);
        CS_vx *= speed*len;
        CS_vy *= speed*len;
    }

    as.vx = CS_vx;
    as.vy = CS_vy;

    // need distance from ground
    const float max_jetpack_height = 8.0f;
    const float jetpack_velocity_max = z_jetpack * 5;
    if (jetpack)
    {
        if (dist_from_ground < max_jetpack_height)
        {   // cap jetpack velocity
            if (as.vz <= jetpack_velocity_max)
                as.vz += z_jetpack;
        }
        else
        if (dist_from_ground < max_jetpack_height + 0.3f)
            as.vz = -z_gravity;
    }

    if (dist_from_ground < 0.025f)
        as.vz -= (new_z - solid_z);
    else
        as.vz += z_gravity;

    #if ADVANCED_JUMP
    float new_jump_pow = as.jump_pow;
    if (jump)
    {
        as.vz = 0.0f;
        new_jump_pow = JUMP_POWINITIAL;
    }
    if (new_jump_pow >= 0)
    {
        as.vz += new_jump_pow;
        new_jump_pow -= JUMP_POWDEC;
    }
    #else
    if (jump)
    {
        as.vz = 0.0f;
        as.vz += JUMP_POW;
    }
    #endif

    as.x = translate_point(new_x);
    as.y = translate_point(new_y);
    if (new_z < solid_z) new_z = solid_z;
    as.z = new_z;
    as.theta = _cs.theta;
    as.phi = _cs.phi;

    return as;
}

void Agent::handle_control_state(int seq, int cs, float theta, float phi)
{
    this->cs[seq].seq = seq;
    this->cs[seq].cs = cs;
    this->cs[seq].theta = theta;
    this->cs[seq].phi = phi;

    tick();

    #if DC_SERVER
    if (client_id != NULL_CLIENT)
    {
        class PlayerAgent_Snapshot P;

        P.id = id;
        //P.seq = (CS_seq+1) % 256;
        P.seq = CS_seq;

        P.x = s.x;
        P.y = s.y;
        P.z = s.z;
        P.vx = s.vx;
        P.vy = s.vy;
        P.vz = s.vz;

        P.theta = s.theta;
        P.phi = s.phi;

        P.sendToClient(client_id);
    }

    if (seq % 32 == 0 )
    {
        class Agent_state_message A;

        //P.seq = (CS_seq+1) % 256;
        A.seq = CS_seq;

        A.x = s.x;
        A.y = s.y;
        A.z = s.z;
        A.vx = s.vx;
        A.vy = s.vy;
        A.vz = s.vz;

        A.theta = s.theta;
        A.phi = s.phi;
        A.id = this->id;
        A.broadcast();

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
void Agent::handle_state_snapshot(int seq,
                                    float theta, float phi,
                                    float x,float y,float z,
                                    float vx,float vy,float vz)
{
    state_snapshot.seq = seq;
    state_snapshot.theta = theta;
    state_snapshot.phi = phi;
    state_snapshot.x=x;state_snapshot.y=y;state_snapshot.z=z;
    state_snapshot.vx=vx;state_snapshot.vy=vy;state_snapshot.vz=vz;

    for (int i=16; i<96; i++)
    {
        int index = (seq + i)%256;
        cs[index].seq = -1;
    }

    state_rollback = state_snapshot; //when new snapshot comes, in, set rollbacks
    CS_seq = seq;

    s = state_snapshot;

    tick();
}
#endif

void Agent::set_position(float x, float y, float z)
{
    s.x = translate_point(x);
    s.y = translate_point(y);
    s.z = z;
}

void Agent::set_state(float  x, float y, float z, float vx, float vy, float vz)
{
    this->set_position(x,y,z);
    s.vx = vx;
    s.vy = vy;
    s.vz = vz;
}

void Agent::set_state_snapshot(float  x, float y, float z, float vx, float vy, float vz)
{
    state_snapshot.x = translate_point(x);
    state_snapshot.y = translate_point(y);
    state_snapshot.z = z;
    state_snapshot.vx = vx;
    state_snapshot.vy = vy;
    state_snapshot.vz = vz;
}

void Agent::set_angles(float theta, float phi)
{
    s.theta = theta;
    s.phi = phi;
}

#if DC_SERVER
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
    struct Vec3 default_spawn = vec3_init(t_map::map_dim.x/2, t_map::map_dim.y/2, t_map::map_dim.z-1);

    float fh = this->current_height();

    if (this->status.spawner == BASE_SPAWN_ID)
    {
        Entities::Entity* base = Entities::get(OBJECT_BASE, 0);
        GS_ASSERT(base != NULL);
        if (base == NULL)
        {
            *spawn = default_spawn;
            return;
        }
        using Components::AgentSpawnerComponent;
        AgentSpawnerComponent* spawner = (AgentSpawnerComponent*)base->get_component(COMPONENT_AGENT_SPAWNER);
        GS_ASSERT(spawner != NULL);
        if (spawner == NULL) *spawn = default_spawn;
        else *spawn = spawner->get_spawn_point(fh, this->box.box_r);
    }
    else // spawner was found
    {
        Entities::Entity* spawner = Entities::get(OBJECT_AGENT_SPAWNER, this->status.spawner);
        GS_ASSERT(spawner != NULL);
        if (spawner != NULL)
        {
            using Components::AgentSpawnerComponent;
            AgentSpawnerComponent* spawner_comp = (AgentSpawnerComponent*)spawner->get_component(COMPONENT_AGENT_SPAWNER);
            GS_ASSERT(spawner_comp != NULL);
            if (spawner_comp != NULL)
                *spawn = spawner_comp->get_spawn_point(fh, this->box.box_r);
            else
                *spawn = default_spawn;
        }
        else
            *spawn = default_spawn;
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
    teleport(p.x, p.y, p.z, 0, 0, 0, theta, 0.0f);
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
    box.b_height = AGENT_HEIGHT;
    box.c_height = AGENT_HEIGHT_CROUCHED;
    box.box_r = AGENT_BOX_RADIUS;

    CS_seq = 0;

    printf("Agent::Agent, new agent, id=%i \n", id);

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
    set_position(0,0,-256);
}

Agent::~Agent()
{
    if (this->vox != NULL) delete this->vox;
}


void Agent::revert_to_snapshot()
{
    s = state_snapshot;
    CS_seq = state_snapshot.seq;
}

void Agent::revert_to_rollback()
{
    s = state_rollback;
    CS_seq = state_rollback.seq;
}

void Agent::print_cs()
{
    uint16_t cs = this->cs[this->CS_seq].cs;
    int forward     = cs & CS_FORWARD ? 1 :0;
    int backwards   = cs & CS_BACKWARD ? 1 :0;
    int left        = cs & CS_LEFT ? 1 :0;
    int right       = cs & CS_RIGHT ? 1 :0;
    int jetpack     = cs & CS_JETPACK ? 1 :0;
    int jump        = cs & CS_JUMP ? 1 :0;
    int crouch      = cs & CS_CROUCH ? 1 :0;
    int boost       = cs & CS_BOOST ? 1 :0;
    int misc1       = cs & CS_MISC1 ? 1 :0;
    int misc2       = cs & CS_MISC2 ? 1 :0;
    int misc3       = cs & CS_MISC3 ? 1 :0;

    printf("f,b,l,r = %d%d%d%d\n", forward, backwards, left, right);
    printf("jet=%d\n", jetpack);
    printf("jump=%d\n", jump);
    printf("crouch=%d\n", crouch);
    printf("boost=%d\n", boost);
    printf("misc123= %d%d%d\n", misc1, misc2, misc3);
}

AgentControlState Agent::get_current_control_state()
{
    return this->cs[(this->CS_seq-1+256)%256];
}

int Agent::crouched()
{
    return this->get_current_control_state().cs & CS_CROUCH;
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
        return this->box.c_height;
    else
        return this->box.b_height;
}

int Agent::current_height_int()
{
    float h = this->current_height();
    return (int)ceil(h);
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
    return data.get_cube_id();
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
    bool forward = (cs.cs & CS_FORWARD) !=0;
    bool backward = (cs.cs & CS_BACKWARD) !=0;
    bool left = (cs.cs & CS_LEFT) !=0;
    bool right = (cs.cs & CS_RIGHT) !=0;

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

    if (sphere_fulstrum_test_translate(center.x, center.y, center.z, radius) == false)
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
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return false;
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
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
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
    return data.get_cube_id();
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
