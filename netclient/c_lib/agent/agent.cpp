#include "agent.hpp"

#ifdef DC_CLIENT
#include <c_lib/compat_gl.h>
#endif

#include <agent/agent_vox.hpp>

#include <c_lib/agent/net_agent.hpp>
#include <c_lib/agent/agent_physics.hpp>

#ifdef DC_CLIENT
#include <c_lib/agent/client/agent_draw.hpp>
#include <c_lib/agent/client/player_agent.hpp>
#endif

#include <c_lib/defines.h>
#include <math.h>
#include <c_lib/common/random.h>

AgentState::AgentState()
:
seq(-1),
theta(0), phi(0),
x(0), y(0), z(0),
vx(0), vy(0), vz(0),
jump_pow(0)
{}

void AgentState::forward_vector(float f[3])
{

    float xa = theta;
    float ya = phi;
    if (theta > 1.0f)
        xa -= 2.0f;
    else if (theta < -1.0f)
        xa += 2.0f;

    // DO NOT ADD ANY MORE SIGNIFICANT DIGITS TO 0.4999f
    // Camera behavior when looking straight up or down is fucked up otherwise
    if (phi > 0.4999f)
        ya = 0.4999f;
    else if (phi < -0.4999f)
        ya = -0.4999f;
    
    f[0] = cos(xa * PI) * cos(ya * PI);
    f[1] = sin(xa * PI) * cos(ya * PI);
    f[2] = sin(ya * PI);

    // normalize
    float len = sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    f[0] /= len;
    f[1] /= len;
    f[2] /= len;
}

Vec3 AgentState::forward_vector()
{
    float f[3];
    this->forward_vector(f);
    return vec3_init(f[0], f[1], f[2]);
}

bool Agent_state::is_you()
{
    bool is = false;
    #ifdef DC_CLIENT
    if (id == ClientState::playerAgent_state.agent_id)
        is = true;
    #endif
    return is;
}

void Agent_state::teleport(float x,float y,float z)
{
    s.x = x;
    s.y = y;
    s.z = z;

    Agent_teleport_message A;

    A.id = id;

    A.x = s.x;
    A.y = s.y;
    A.z = s.z;
    A.vx = s.vx;
    A.vy = s.vy;
    A.vz = s.vz;
    A.theta = s.theta;
    A.phi = s.phi;
    A.broadcast();
}

void Agent_state::teleport(float x,float y,float z, float vx, float vy, float vz, float theta, float phi)
{
    s.x = x;
    s.y = y;
    s.z = z;
    s.vx = vx;
    s.vy = vy;
    s.vz = vz;

    s.theta = theta;
    s.phi = phi;
    
    Agent_teleport_message A;

    A.id = id;

    A.x = s.x;
    A.y = s.y;
    A.z = s.z;
    A.vx = s.vx;
    A.vy = s.vy;
    A.vz = s.vz;
    A.theta = s.theta;
    A.phi = s.phi;
    A.broadcast();
}

void Agent_state::tick() 
{
    if (this->status.team != 0)
    {   // dont apply physics to viewers
        int _tc =0;
        struct Agent_control_state _cs;

        while(cs[CS_seq].seq == CS_seq )
        {
            _cs = cs[CS_seq];
            s = _agent_tick(_cs, box, s);

            CS_seq = (CS_seq+1)%256;
            _tc++;
        }
    }

    #ifdef DC_SERVER
    const float Z_DEATH_ZONE = -200.0f;
    if (!this->status.dead && this->s.z < Z_DEATH_ZONE)
        this->status.die(this->id, OBJECT_AGENT, DEATH_BELOW_MAP);
    else
        this->status.respawn();
    this->status.tick();
    #endif
}


#define ADVANCED_JUMP 0
//#define ADVANCED_JETPACK 0

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

    const float z_gravity = -3.0f / tr2;
    const float z_jetpack = (1.0f / tr2) - z_gravity;

#if ADVANCED_JUMP
    const float JUMP_POWINITIAL = 1 * 0.17;
    const float JUMP_POWDEC = 0.2 * 0.24;
#else
    const float JUMP_POW = 1 * 0.40;
#endif
    //const float z_bounce = 0.10f;
    //const float z_bounce_v_threshold = 1.5f / tr;

    const float pi = 3.14159265f;

    float CS_vx = 0;
    float CS_vy = 0;

    if(forward)
    {
            CS_vx += speed*cos( _cs.theta * pi);
            CS_vy += speed*sin( _cs.theta * pi);
    }
    if(backwards)
    {
            CS_vx += -speed*cos( _cs.theta * pi);
            CS_vy += -speed*sin( _cs.theta * pi);
    }
    if(left) 
    {
            CS_vx += speed*cos( _cs.theta * pi + pi/2);
            CS_vy += speed*sin( _cs.theta * pi + pi/2);
    }
    if(right) 
    {
            CS_vx += -speed*cos( _cs.theta * pi + pi/2);
            CS_vy += -speed*sin( _cs.theta * pi + pi/2);
    }

    const float precision = 0.000001f;
    // normalize diagonal motion
    if (CS_vx < -precision || CS_vx > precision || CS_vy < -precision || CS_vy > precision)
    {
        float len = sqrt(CS_vx*CS_vx + CS_vy*CS_vy);
        CS_vx *= speed/len;
        CS_vy *= speed/len;
    }

    as.vx = CS_vx;
    as.vy = CS_vy;

    // need distance from ground
    //const float max_jetpack_height = 2.2f;
    const float max_jetpack_height = 8.0f;
    const float jetpack_velocity_max = z_jetpack * 10;
    float dist_from_ground = as.z - (_get_highest_open_block(as.x, as.y)-1); //TODO: use a function like this that takes a starting z point
    if (jetpack)
    {
        if (dist_from_ground < max_jetpack_height)
        {   // cap jetpack velocity
            if (as.vz <= jetpack_velocity_max)
                as.vz += z_jetpack;
            else
                as.vz = -z_gravity;
        }
        else if (dist_from_ground < max_jetpack_height + 0.3f)
            as.vz = -z_gravity;
    }

    //jet pack and gravity
    //if(as.z > 0)
    //{
        as.vz += z_gravity;
    //} 
    //else // under the map, go back up
    //{
        //as.vz -= z_gravity;
    //}    

#if ADVANCED_JUMP
    float new_jump_pow = as.jump_pow;
    if (jump)
    {
        as.vz = 0;
        new_jump_pow = JUMP_POWINITIAL;
    }
    if (new_jump_pow >= 0) {
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

    float new_x, new_y, new_z;
    new_x = as.x + as.vx + CS_vx;
    new_y = as.y + as.vy + CS_vy;
    new_z = as.z + as.vz;
    //collision
    bool current_collision = collision_check_final_current(box.box_r, height, as.x,as.y,as.z);
    if(current_collision) {
        //printf("current\n");
        as.x = new_x;
        as.y = new_y;
        as.z += 0.02f; //nudge factor
        if(as.vz < 0.0f) as.vz = 0.0f;

        return as;
    }


    /*
        Collision Order: as.x,as.y,as.z
    */
    bool collision_x = collision_check_final_xy(box.box_r, height, new_x,as.y,as.z);
    if(collision_x) {
        //printf("x\n");
        new_x = as.x;
        as.vx = 0.0f;
    }

    bool collision_y = collision_check_final_xy(box.box_r, height, new_x,new_y,as.z);
    if(collision_y) {
        //printf("y\n");
        new_y = as.y;
        as.vy = 0.0f;
    }

    //top and bottom matter
    bool top=false;
    bool collision_z = collision_check_final_z(box.box_r, height, new_x, new_y, new_z, &top);
    if(collision_z) {
        //printf("z\n");
        new_z = as.z;
        if (top)
            new_z = floor(as.z) + ceil(height) - height;
        as.vz = 0.0f;
    }       


    as.x = new_x;
    as.y = new_y;
    as.z = new_z;

#if ADVANCED_JUMP
    as.jump_pow = new_jump_pow;
#endif

    as.theta = _cs.theta;
    as.phi = _cs.phi;

    return as;
}

void Agent_state::handle_control_state(int seq, int cs, float theta, float phi) {

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

    #ifdef DC_SERVER
    
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
        for(int i=16; i<96; i++)
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

    for(int i=16; i<96; i++)
    {
        int index = (seq + i)%256;
        cs[index].seq = -1;
    }
                
    state_rollback = state_snapshot; //when new snapshot comes, in, set rollbacks
    CS_seq = seq;

    s = state_snapshot;

    //#ifdef DC_CLIENT
    //AgentDraw::add_snapshot_to_history(this);
    //#endif

    tick();
    #else
    printf("ERROR: Server is calling handle_state_snapshot\n");
    #endif
}

void Agent_state::set_state(float  x, float y, float z, float vx, float vy, float vz)
{
    s.x = x;
    s.y = y;
    s.z = z;
    s.vx = vx;
    s.vy = vy;
    s.vz = vz;
}

void Agent_state::set_angles(float theta, float phi)
{
    s.theta = theta;
    s.phi = phi;
}

void Agent_state::get_spawn_point(Vec3* spawn)
{
    if (this->status.team == 0)
    {
        spawn->x = map_dim.x/2;
        spawn->y = map_dim.y/2;
        spawn->z = map_dim.z - 1;
        return;
    }
    
    using Components::BASE_SPAWN_ID;

    int h = this->current_height_int();
    Components::AgentSpawnerComponent *s = NULL;
    
    if (this->status.spawner != BASE_SPAWN_ID)
    {    // check that assigned spawner still exists, reassign if not
        while ((s = Components::agent_spawner_component_list->get_by_team_index(this->status.team, this->status.spawner)) == NULL)
        {
            this->status.set_spawner();
            if (this->status.spawner == BASE_SPAWN_ID) break;  // no spawners available
        }
    }

    if (this->status.spawner == BASE_SPAWN_ID)
    {   // spawner is base
        STATE::ctf->get_base_spawn_point(this->status.team, h, spawn);

        // team is 0, or spawn get failed for some reason. spawn anywhere
        if (spawn == NULL)
        {
            int x,y,z;
            x = randrange(0, map_dim.x-1);
            y = randrange(0, map_dim.y-1);
            z = _get_highest_open_block(x,y, h);
            spawn->x = x;
            spawn->y = y;
            spawn->z = z;
        }
    }
    else // spawner was found
        s->get_spawn_point(h, spawn);
}

void Agent_state::spawn_state()
{   // update position
    Vec3 spawn;
    this->get_spawn_point(&spawn);
    float theta = this->status.get_spawn_angle();
    teleport(spawn.x, spawn.y, spawn.z, 0, 0, 0, theta, 0.0f);
}

void Agent_state::init_vox()
{
    this->vox = new Voxel_model(&VoxDats::agent, this->id, this->type);
    this->vox->set_hitscan(true);
    this->vox->register_hitscan();
}

Agent_state::Agent_state(int id)
:
id (id), type(OBJECT_AGENT), status(this), weapons(this)
#ifdef DC_CLIENT
, event(this)
#endif
{
    inventory_id = -1;

    set_state(16.5f, 16.5f, 16.5f, 0.0f, 0.0f, 0.0f);
    set_angles(0.5f, 0.0f);

    box.b_height = AGENT_HEIGHT;
    box.c_height = AGENT_HEIGHT_CROUCHED;
    box.box_r = AGENT_BOX_RADIUS;

    CS_seq = 0;

    printf("Agent_state::Agent_state, new agent, id=%i \n", id);
    
    state_snapshot.seq = -1;
    state_rollback.seq = -1;
    for(int i=0; i<256; i++)
    {
        cs[i].seq = -1;
        cs[i].cs = 0;
        cs[i].theta = 0;
        cs[i].phi = 0;
    }

    client_id = id;

    // add to NetServer pool
    //NetServer::assign_agent_to_client(this->client_id, this);

    #ifdef DC_SERVER
    agent_create_StoC msg;
    msg.id = id;
    msg.team = this->status.team;
    msg.client_id = this->client_id;
    msg.broadcast();
    #endif

    this->init_vox();
}

Agent_state::Agent_state(int id, float x, float y, float z, float vx, float vy, float vz)
:
id(id), type(OBJECT_AGENT), status(this), weapons(this)
#ifdef DC_CLIENT
, event(this)
#endif
{
    inventory_id = -1;

    set_state(x, y, z, vx, vy, vz);
    set_angles(0.5f, 0.0f);
    
    box.b_height = AGENT_HEIGHT;
    box.c_height = AGENT_HEIGHT_CROUCHED;
    box.box_r = AGENT_BOX_RADIUS;

    CS_seq = 0;

    printf("Agent_state::Agent_state, new agent, id=%i \n", id);
    
    state_snapshot.seq = -1;
    state_rollback.seq = -1;
    for(int i=0; i<256; i++)
    {
        cs[i].seq = -1;
        cs[i].cs = 0;
        cs[i].theta = 0;
        cs[i].phi = 0;
    }


    client_id = id;

    #ifdef DC_SERVER
    agent_create_StoC msg;
    msg.id = id;
    msg.team = this->status.team;
    msg.client_id = this->client_id;
    msg.broadcast();
    #endif

    this->init_vox();
}

Agent_state::~Agent_state()
{
    #ifdef DC_SERVER
    agent_destroy_StoC msg;
    msg.id = id;
    msg.broadcast();
    #endif

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
        return CAMERA_HEIGHT_DEAD
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

int Agent_state::get_facing_block_type()
{
    const float CUBE_SELECT_MAX_DISTANCE = 12.0f;
    const int z_low = 8;
    const int z_high = 8;
    Vec3 f;
    float x,y,z;
    
#ifdef DC_CLIENT
    if (ClientState::playerAgent_state.you == this)
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
    return _get(pos[0], pos[1], pos[2]);
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
    return this->in_sight_of(source, sink, 0.0f);
}

bool Agent_state::in_sight_of(Vec3 source, Vec3* sink, float acquisition_probability)
{
    if (this->vox == NULL) return false;
    return this->vox->in_sight_of(source, sink, acquisition_probability);
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
    bool forward = (cs.cs & CS_FORWARD);
    bool backward = (cs.cs & CS_BACKWARD);
    bool left = (cs.cs & CS_LEFT);
    bool right = (cs.cs & CS_RIGHT);

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
    
    this->vox->was_updated = false;
    if (current_camera->first_person && this->is_you())
    {   // your agent
        this->vox->set_draw(false);
        this->vox->set_hitscan(false);
        this->vox->update(this->s.x, this->s.y, this->s.z, this->s.theta, this->s.phi);
        return;
    }

    // other agents
    VoxDat* vox_dat = &VoxDats::agent;
    Vec3 center = this->vox->get_part(0)->get_center();
    float radius = this->vox->get_part(0)->radius;
    if (sphere_fulstrum_test(center.x, center.y, center.z, radius) == false)
    {   // agent not in view fulcrum
        this->vox->set_draw(false);
        this->vox->set_hitscan(false);
        if (this->event.bb != NULL)
            this->event.bb->set_draw(false);
        return;
    }
    if (this->event.bb != NULL)
        this->event.bb->set_draw(true);
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
    if (this->status.dead)
        vox_dat = &VoxDats::agent_dead;
        
    this->vox->set_vox_dat(vox_dat);
    this->update_legs();
    this->vox->update(this->s.x, this->s.y, this->s.z, this->s.theta, this->s.phi);
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
    if (this->status.dead)
        vox_dat = &VoxDats::agent_dead;

    this->vox->set_vox_dat(vox_dat);
    this->update_legs();
    this->vox->update(this->s.x, this->s.y, this->s.z, this->s.theta, this->s.phi);
    this->vox->set_hitscan(true);
    #endif
}

bool Agent_state::near_base()
{
            bool near_base();   // is within the base's spawn radius
    Base* b = STATE::ctf->get_base(this->status.team);
    if (b == NULL) return false;
    if (distancef_squared(b->x, b->y, b->z, this->s.x, this->s.y, this->s.z) < BASE_SPAWN_RADIUS*BASE_SPAWN_RADIUS)
        return true;
    return false;
}
