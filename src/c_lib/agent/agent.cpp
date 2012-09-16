#include "agent.hpp"

#if DC_CLIENT
#include <common/compat_gl.h>
#endif

#include <agent/net_agent.hpp>
#include <agent/agent_physics.hpp>

#if DC_CLIENT
#include <agent/client/agent_draw.hpp>
#include <agent/client/player_agent.hpp>
#endif

#include <common/defines.h>
#include <math.h>
#include <common/random.h>

#include <item/toolbelt/_interface.hpp>
#include <item/common/constant.hpp>

#include <physics/quadrant.hpp>

AgentState::AgentState()
:
seq(-1),
theta(0), phi(0),
x(0), y(0), z(0),
vx(0), vy(0), vz(0)
//,jump_pow(0)
{}

Vec3 AgentState::forward_vector()
{
    if (theta > 1.0f)
    {
        theta -= 2.0f;
        GS_ASSERT(false);
    }
    if (theta < -1.0f)
    {
        theta += 2.0f;
        GS_ASSERT(false);
    }

    if (phi > 0.4999f)
    {
        phi = 0.4999f;
        GS_ASSERT(false);
    }
    if (phi < -0.4999f)
    {
        phi = -0.4999f;
        GS_ASSERT(false);
    }

    Vec3 f = vec3_init(1.0f, 0.0f, 0.0f);
    f = vec3_euler_rotation(f, theta, phi, 0.0f);
    normalize_vector(&f);

    return f;
}

#if DC_CLIENT
bool Agent_state::is_you()
{
    if (id == ClientState::playerAgent_state.agent_id) return true;
    return false;
}
#endif

void Agent_state::teleport(float x,float y,float z)
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

    t_map::t_map_manager_update_client_position(this->id, x,y);
    #endif
}

void Agent_state::teleport(float x,float y,float z, float vx, float vy, float vz, float theta, float phi)
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

    t_map::t_map_manager_update_client_position(this->id, x,y);
    #endif
}

void Agent_state::tick() 
{
    int _tc =0;
    struct Agent_control_state _cs;

    while (cs[CS_seq].seq == CS_seq )
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

//takes an agent state and control state and returns new agent state
class AgentState _agent_tick(const struct Agent_control_state _cs, const struct Agent_collision_box box, class AgentState as)
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

    const float tr = 10.0f;    //tick rate
    const float tr2 = tr*tr;

    float speed = AGENT_SPEED / tr;
    float height = box.b_height;
    if (crouch)
    {
        speed = AGENT_SPEED_CROUCHED / tr;
        height = box.c_height;
    }

    float z_gravity = -3.0f * (1.0f / tr2);
    #if DC_CLIENT
    if (!t_map::position_is_loaded(as.x, as.y)) z_gravity = 0.0f;
    #endif

    const float z_jetpack = (1.0f / tr2) - z_gravity;

    #if ADVANCED_JUMP
    const float JUMP_POWINITIAL = 1.0f * 0.17f;
    const float JUMP_POWDEC = 0.2f * 0.24f;
    #else
    const float JUMP_POW = AGENT_JUMP_POWER;
    #endif
    //const float z_bounce = 0.10f;
    //const float z_bounce_v_threshold = 1.5f / tr;

    const float pi = 3.14159265f;

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
    float solid_z = (float)t_map::get_solid_block_below(as.x, as.y, as.z);
    float dist_from_ground = as.z - solid_z;
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

    //as.vz += z_gravity;

    if (dist_from_ground < 0.025f)
        as.vz -= (as.z - solid_z);
    else
        as.vz += z_gravity;
        
    #if ADVANCED_JUMP
    float new_jump_pow = as.jump_pow;
    if (jump)
    {
        as.vz = 0;
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

    float new_x = as.x + as.vx + CS_vx;
    float new_y = as.y + as.vy + CS_vy;
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


    /*
        Collision Order: as.x,as.y,as.z
    */
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

    as.x = translate_point(new_x);
    as.y = translate_point(new_y);
    if (new_z < solid_z) new_z = solid_z;
    as.z = new_z;

    #if ADVANCED_JUMP
    as.jump_pow = new_jump_pow;
    #endif

    as.theta = _cs.theta;
    as.phi = _cs.phi;

    return as;
}

void Agent_state::handle_control_state(int seq, int cs, float theta, float phi)
{

    //printf("cs= %i \n", seq);


    //if (seq != CS_seq) printf("seq != CS_seq: %i %i  \n", seq, CS_seq);

    //int index = seq%256;

    this->cs[seq].seq = seq;
    this->cs[seq].cs = cs;
    this->cs[seq].theta = theta;
    this->cs[seq].phi = phi;

    //printf("index= %i \n", index);

    //printf("1 seq: %i \n", seq);
    //printf("1 CS_seq: %i \n", CS_seq);
    tick();
    //printf("2 seq: %i \n", seq);
    //printf("2 CS_seq: %i \n", CS_seq);

    #if DC_SERVER
    
    if (client_id != -1) 
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

void Agent_state::handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz) {
    #if DC_CLIENT
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

    //#if DC_CLIENT
    //AgentDraw::add_snapshot_to_history(this);
    //#endif

    tick();
    #else
    printf("ERROR: Server is calling handle_state_snapshot\n");
    #endif
}

void Agent_state::set_position(float x, float y, float z)
{
    s.x = translate_point(x);
    s.y = translate_point(y);
    s.z = z;
}

void Agent_state::set_state(float  x, float y, float z, float vx, float vy, float vz)
{
    this->set_position(x,y,z);
    s.vx = vx;
    s.vy = vy;
    s.vz = vz;
}

void Agent_state::set_state_snapshot(float  x, float y, float z, float vx, float vy, float vz)
{
    state_snapshot.x = translate_point(x);
    state_snapshot.y = translate_point(y);
    state_snapshot.z = z;
    state_snapshot.vx = vx;
    state_snapshot.vy = vy;
    state_snapshot.vz = vz;
}

void Agent_state::set_angles(float theta, float phi)
{
    s.theta = theta;
    s.phi = phi;
}

#if DC_SERVER
void Agent_state::set_camera_state(float x, float y, float z, float theta, float phi)
{
    this->camera.x = translate_point(x);
    this->camera.y = translate_point(y);
    this->camera.z = z;
    this->camera.theta = theta;
    this->camera.phi = phi;
}

void Agent_state::get_spawn_point(Vec3* spawn)
{
    Vec3 default_spawn = vec3_init(map_dim.x/2, map_dim.y/2, map_dim.z-1);

    float fh = this->current_height();

    if (this->status.spawner == BASE_SPAWN_ID)
    {
        Objects::Object* base = Objects::get(OBJECT_BASE, 0);
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
        Objects::Object* spawner = Objects::get(OBJECT_AGENT_SPAWNER, this->status.spawner);
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

void Agent_state::spawn_state()
{   // update position
    Vec3 spawn;
    this->get_spawn_point(&spawn);
    this->spawn_state(spawn);
}

void Agent_state::spawn_state(Vec3 p)
{   // update position
    float theta = this->status.get_spawn_angle();
    teleport(p.x, p.y, p.z, 0, 0, 0, theta, 0.0f);
}
#endif

void Agent_state::init_vox()
{
    this->vox = new Voxel_model(&VoxDats::agent, this->id, this->type);
    this->vox->set_hitscan(true);
    this->vox->register_hitscan();
}

Agent_state::Agent_state(int id)
:
id (id), type(OBJECT_AGENT), status(this)
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

    printf("Agent_state::Agent_state, new agent, id=%i \n", id);
    
    state_snapshot.seq = -1;
    state_rollback.seq = -1;
    for (int i=0; i<256; i++)
    {
        cs[i].seq = -1;
        cs[i].cs = 0;
        cs[i].theta = 0;
        cs[i].phi = 0;
    }

    client_id = id;

    set_angles(0.5f, 0.0f);
    set_position(0,0,-256);

    this->init_vox();
}

Agent_state::~Agent_state()
{
    #if DC_SERVER
    agent_destroy_StoC msg;
    msg.id = id;
    msg.broadcast();
    #endif

    Toolbelt::remove_agent(this->id);

    if (this->vox != NULL) delete this->vox;
}


void Agent_state::revert_to_snapshot() {
    s = state_snapshot;
    CS_seq = state_snapshot.seq;
}

void Agent_state::revert_to_rollback() {
    s = state_rollback;            
    CS_seq = state_rollback.seq;
}

void Agent_state::print_cs()
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

Agent_control_state Agent_state::get_current_control_state()
{
    return this->cs[(this->CS_seq-1+256)%256];
}

int Agent_state::crouched()
{
    return this->get_current_control_state().cs & CS_CROUCH;
}

float Agent_state::camera_height()
{
    if (this->status.dead)
        return CAMERA_HEIGHT_DEAD;
    else if (this->crouched())
        return CAMERA_HEIGHT_CROUCHED;
    return CAMERA_HEIGHT;
}

float Agent_state::current_height()
{
    if (this->crouched())
        return this->box.c_height;
    else
        return this->box.b_height;
}

int Agent_state::current_height_int()
{
    float h = this->current_height();
    return (int)ceil(h);
}

float Agent_state::camera_z()
{
    return this->s.z + this->camera_height();
}

Vec3 Agent_state::camera_position()
{
    return vec3_init(this->s.x, this->s.y, this->camera_z());
}

class Voxel_volume* Agent_state::get_arm()
{
    if (this->vox == NULL) return NULL;
    return this->vox->get_part(AGENT_PART_RARM);
}

Vec3 Agent_state::arm_center()
{
    if (this->vox == NULL || !this->vox->was_updated)
        return this->get_center();
    return this->vox->get_part(AGENT_PART_RARM)->get_center();
}

Vec3 Agent_state::arm_forward()
{
    if (this->vox == NULL) return vec3_init(1,0,0);
    return this->vox->get_part(AGENT_PART_RARM)->world_matrix.vx;
}

Vec3 Agent_state::arm_right()
{
    if (this->vox == NULL) return vec3_init(0,1,0);
    return this->vox->get_part(AGENT_PART_RARM)->world_matrix.vy;
}

Vec3 Agent_state::arm_up()
{
    if (this->vox == NULL) return vec3_init(0,0,1);
    return this->vox->get_part(AGENT_PART_RARM)->world_matrix.vz;
}

int Agent_state::get_facing_block_type()
{
    const float CUBE_SELECT_MAX_DISTANCE = 12.0f;
    const int z_low = 8;
    const int z_high = 8;
    Vec3 f;
    float x,y,z;
    
    #if DC_CLIENT
    if (this->is_you())
    {   // if you, use camera / player agent state instead.
        f = agent_camera->forward_vector();
        x = ClientState::playerAgent_state.camera_state.x;
        y = ClientState::playerAgent_state.camera_state.y;
        z = ClientState::playerAgent_state.camera_z();
    }
    else
    #endif
    {
        f = this->s.forward_vector();
        x = this->s.x;
        y = this->s.y;
        z = this->camera_z();
    }
        
    int *pos = _nearest_block(
        x, y, z,
        f.x, f.y, f.z,
        CUBE_SELECT_MAX_DISTANCE,
        z_low, z_high
    );
    if (pos == NULL) return 0;
    return t_map::get(pos[0], pos[1], pos[2]);
}

bool Agent_state::point_can_cast(float x, float y, float z, float max_dist)
{
    // checks if a point can raycast to some area of the agent box,
    // or if the terrain prevents it

    // sample points on the outside of the agent box
    const int samples_per_height = 8;
    float step = this->current_height() / samples_per_height;

    int i;
    float a,b,c;

    // check the center column
    a = this->s.x;
    b = this->s.y;
    for (i=0; i<samples_per_height; i++)
    {
        c = this->s.z + i*step;
        if (ray_cast_simple(x,y,z, a,b,c, max_dist))
            return true;
    }

    // check the 4 corner columns
    a = this->s.x + this->box.box_r;
    b = this->s.y + this->box.box_r;
    for (i=0; i<samples_per_height; i++)
    {
        c = this->s.z + i*step;
        if (ray_cast_simple(x,y,z, a,b,c, max_dist))
            return true;
    }
    
    a = this->s.x + this->box.box_r;
    b = this->s.y - this->box.box_r;
    for (i=0; i<samples_per_height; i++)
    {
        c = this->s.z + i*step;
        if (ray_cast_simple(x,y,z, a,b,c, max_dist))
            return true;
    }
    a = this->s.x - this->box.box_r;
    b = this->s.y + this->box.box_r;
    for (i=0; i<samples_per_height; i++)
    {
        c = this->s.z + i*step;
        if (ray_cast_simple(x,y,z, a,b,c, max_dist))
            return true;
    }
    a = this->s.x - this->box.box_r;
    b = this->s.y - this->box.box_r;
    for (i=0; i<samples_per_height; i++)
    {
        c = this->s.z + i*step;
        if (ray_cast_simple(x,y,z, a,b,c, max_dist))
            return true;
    }

    return false;
}

bool Agent_state::in_sight_of(Vec3 source, Vec3* sink)
{
    if (this->vox == NULL) return false;
    return this->vox->in_sight_of(source, sink);
}

bool Agent_state::in_sight_of(Vec3 source, Vec3* sink, float failure_rate)
{
    if (this->vox == NULL) return false;
    return this->vox->in_sight_of(source, sink, failure_rate);
}

void Agent_state::update_legs()
{
    const float arc = 25.0f;    // degree
    const int peak = 37;
    const int rest = (peak-1)/2;
    const float m = (arc/180)/((float)rest);

    static int idle = 0;
    static int legtick = 0;
    static float direction = -1;

    Agent_control_state cs = this->get_current_control_state();
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

void Agent_state::update_model()
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
        ClientState::playerAgent_state.update_model();
        return;
    }

    Vec3 center = this->get_center();    // model is fresh, use model center for more accurate culling
        
    this->vox->was_updated = false;
    // other agents
    VoxDat* vox_dat = &VoxDats::agent;
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
    VoxDat* vox_dat = &VoxDats::agent;
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

bool Agent_state::near_base()
{
    Objects::Object* b = Objects::get(OBJECT_BASE, 0);
    if (b == NULL) return false;
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)b->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return false;
    Vec3 bp = physics->get_position();
    
    float x = quadrant_translate_f(this->s.x, bp.x);
    float y = quadrant_translate_f(this->s.y, bp.y);
    float z = bp.z;
    using Objects::BASE_SPAWN_RADIUS;
    static const float k = sqrtf(2);    // scaling factor for radius of a square (base spawning is square manhattan distance)
    if (distancef_squared(x,y,z, this->s.x, this->s.y, this->s.z) < k*BASE_SPAWN_RADIUS*BASE_SPAWN_RADIUS)
        return true;
    return false;
}

int* Agent_state::nearest_open_block(const float max_dist, const int z_low, const int z_high)
{
    Vec3 p = this->get_camera_position();
    Vec3 f = this->forward_vector();
    int* b = _farthest_empty_block(
        p.x, p.y, p.z,
        f.x, f.y, f.z,
        max_dist, z_low, z_high);
    return b;
}

void force_update_agent_vox(Agent_state* a)
{
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->vox->was_updated = false;
    AgentState s = a->get_state();
    a->vox->update(s.x, s.y, s.z, s.theta, -s.phi);
}

// returns block type
int Agent_state::get_facing_side(int solid_pos[3], int open_pos[3], int side[3], float* distance)
{
    Vec3 p = this->get_camera_position();
    Vec3 v = this->forward_vector();
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

// returns side as int
int Agent_state::get_facing_side(int solid_pos[3], int open_pos[3], float* distance)
{
    int s[3];
    int block = this->get_facing_side(solid_pos, open_pos, s, distance);
    if (block <= 0) return -1;
    return get_cube_side_from_side_array(s);
}

// returns side as int
int Agent_state::get_facing_side(int solid_pos[3], int open_pos[3], const float max_distance, const int z_low, const int z_high)
{
    Vec3 p = this->get_camera_position();
    Vec3 v = this->forward_vector();

    int s[3];   
    int* b = _farthest_empty_block(p.x, p.y, p.z, v.x, v.y, v.z, s, max_distance, z_low, z_high);
    if (b == NULL) return -1;

    open_pos[0] = b[0];
    open_pos[1] = b[1];
    open_pos[2] = b[2];
    
    solid_pos[0] = translate_point(open_pos[0] - s[0]);
    solid_pos[1] = translate_point(open_pos[1] - s[1]);
    solid_pos[2] = open_pos[2] - s[2];
    GS_ASSERT(open_pos[2] >= 0); // agent should never be shooting from underground
    GS_ASSERT(solid_pos[2] >= 0); // agent should never be shooting from underground

    return get_cube_side_from_side_array(s);
}
