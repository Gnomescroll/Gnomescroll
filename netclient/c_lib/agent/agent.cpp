#include "agent.hpp"

#include <c_lib/agent/net_agent.hpp>
#include <c_lib/agent/agent_draw.hpp>
#include <c_lib/agent/player_agent.hpp>

#include <c_lib/agent/agent_physics.hpp>

#include <c_lib/defines.h>
#include <math.h>
#include <c_lib/common/random.h>

#include <c_lib/voxel/voxel_volume.hpp>

void Agent_list::draw() 
{
    #ifdef DC_CLIENT

    bool you;
    int i;
    Agent_state* agent;
    for(i=0; i<n_max; i++) {
        agent = a[i];
        if (agent == NULL) continue;
        you = (agent->id == ClientState::playerAgent_state.agent_id);
        if (agent->vox == NULL) continue;
        
        agent->vox->update(&agent_vox_dat, agent->s.x, agent->s.y, agent->s.z, agent->s.theta, agent->s.phi);
        if ((first_person && you) || agent->status.dead)
        {
            agent->vox->set_draw(false);
            agent->vox->set_hitscan(false);
        }
        else
        {
            agent->vox->set_draw(true);
            agent->vox->set_hitscan(true);
        }
    }

    ClientState::voxel_render_list.draw();
    #endif
}

AgentState::AgentState()
:
seq(-1),
theta(0), phi(0),
x(0), y(0), z(0),
vx(0), vy(0), vz(0),
jump_pow(0)
{}

void AgentState::forward_vector(float f[3]) {

    float xa = theta;
    float ya = phi;
    if (theta > 1.0f) {
        xa -= 2.0f;
    } else if (theta < -1.0f) {
        xa += 2.0f;
    }

    // DO NOT ADD ANY MORE SIGNIFICANT DIGITS TO 0.4999f
    // Camera behavior when looking straight up or down is fucked up otherwise
    if (phi > 0.4999f) {
        ya = 0.4999f;
    } else if (phi < -0.4999f) {
        ya = -0.4999f;
    }
    
    f[0] = cos( xa * PI) * cos( ya * PI);
    f[1] = sin( xa * PI) * cos( ya * PI);
    f[2] = sin( ya * PI);

    // normalize?
    float len = sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    f[0] /= len;
    f[1] /= len;
    f[2] /= len;
}

bool Agent_state::is_you() {
    bool is = false;
    #ifdef DC_CLIENT
    if (id == ClientState::playerAgent_state.agent_id) {
        is = true;
    }
    #endif
    return is;
}

void Agent_state::send_id_to_client(int client_id) {
    #ifdef DC_SERVER
    PlayerAgent_id_StoC msg;
    msg.id = id;
    msg.sendToClient(client_id);
    #endif
}

void Agent_state::teleport(float x,float y,float z) {
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

void Agent_state::teleport(float x,float y,float z, float vx, float vy, float vz, float theta, float phi) {
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
    int _tc =0;
    struct Agent_control_state _cs;

    while(cs[(cs_seq+1) % 128].seq == (cs_seq+1)% 256) {

        cs_seq = (cs_seq+1)%256;
        _cs = cs[cs_seq % 128];

        s = _agent_tick(_cs, box, s, this);
        _tc++;
    }

    #ifdef DC_SERVER
    status.respawn();
    #endif
}


//takes an agent state and control state and returns new agent state
class AgentState _agent_tick(const struct Agent_control_state _cs, const struct Agent_collision_box box, class AgentState as, Agent_state* a)
 {
    int a_cs = _cs.cs;
    //set control state variables
    bool forward     = a_cs & 1? 1 :0;
    bool backwards   = a_cs & 2? 1 :0;
    bool left        = a_cs & 4? 1 :0;
    bool right       = a_cs & 8? 1 :0;
    bool jetpack     = a_cs & 16? 1 :0;
    bool jump        = a_cs & 32? 1 :0;
    bool crouch      = a_cs & 64? 1 :0;
    //implemented, but unused
    /*
    bool boost       = a_cs & 128? 1 :0;
    bool misc1       = a_cs & 256? 1 :0;
    bool misc2       = a_cs & 512? 1 :0;
    bool misc3       = a_cs & 1024? 1 :0;     
    */

    const float tr = 10.0f;    //tick rate
    const float tr2 = tr*tr;

    float xy_speed;
    float height;
    xy_speed = 2.0f / tr;
    height = box.b_height;
    if (crouch)
    {
        xy_speed = 0.7 / tr;
        height = box.c_height;
    }


    const float z_gravity = -3.0f / tr2;
    const float z_jetpack = (1.0f / tr2) - z_gravity;

    const float JUMP_POWINITIAL = 1 * 0.17;
    const float JUMP_POWDEC = 0.2 * 0.24;

    const float z_bounce = 0.10f;
    const float z_bounce_v_threshold = 1.5f / tr;

    const float pi = 3.14159265f;

    float cs_vx = 0;
    float cs_vy = 0;

    if(forward)
    {
            cs_vx += xy_speed*cos( _cs.theta * pi);
            cs_vy += xy_speed*sin( _cs.theta * pi);
    }
    if(backwards)
    {
            cs_vx += -xy_speed*cos( _cs.theta * pi);
            cs_vy += -xy_speed*sin( _cs.theta * pi);
    }
    if(left) 
    {
            cs_vx += xy_speed*cos( _cs.theta * pi + pi/2);
            cs_vy += xy_speed*sin( _cs.theta * pi + pi/2);
    }
    if(right) 
    {
            cs_vx += -xy_speed*cos( _cs.theta * pi + pi/2);
            cs_vy += -xy_speed*sin( _cs.theta * pi + pi/2);
    }

    //jet pack and gravity
    if(as.z > 0)
    {
        as.vz += z_gravity;
    } 
    else // under the map, go back up
    {
        as.vz -= z_gravity;
    }    
    
    if (jetpack) {
        as.vz += z_jetpack;
    }

    float new_jump_pow = as.jump_pow;
    if (jump)
    {
        new_jump_pow = JUMP_POWINITIAL;
    }
    if (new_jump_pow >= 0) {
        as.vz += new_jump_pow;
        new_jump_pow -= JUMP_POWDEC;
    }

    float new_x, new_y, new_z;
    new_x = as.x + as.vx + cs_vx;
    new_y = as.y + as.vy + cs_vy;
    new_z = as.z + as.vz;

    //collision
    bool current_collision = collision_check5(box.box_r, height, as.x,as.y,as.z);
    if(current_collision) {
        as.x = new_x;
        as.y = new_y;
        as.z += 0.02f; //nudge factor
        if(as.vz < 0.0f) as.vz = 0.0f;

        return as;
    }

    /*
        Collision Order: as.x,as.y,as.z
    */
    bool collision_x = collision_check5(box.box_r, height, new_x,as.y,as.z);
    if(collision_x) {
        new_x = as.x;
        as.vx = 0.0f;
    }

    bool collision_y = collision_check5(box.box_r, height, new_x,new_y,as.z);
    if(collision_y) {
        new_y = as.y;
        as.vy = 0.0f;
    }

    //top and bottom matter
    bool collision_z = collision_check5(box.box_r, height, new_x,new_y,new_z);
    if(collision_z) {

        if(as.vz < -z_bounce_v_threshold)
        {
            as.vz *= -1.0f *z_bounce;
        }
        else
        {
            as.vz = 0.0f;
        }

        new_z = as.z + as.vz;
    }       

    as.x = new_x;
    as.y = new_y;
    as.z = new_z;
    as.jump_pow = new_jump_pow;

    as.theta = _cs.theta;
    as.phi = _cs.phi;

    return as;
}

void Agent_state::handle_control_state(int seq, int cs, float theta, float phi) {
    int index = seq%128;

    this->cs[index].seq = seq;
    this->cs[index].cs = cs;
    this->cs[index].theta = theta;
    this->cs[index].phi = phi;

    tick();

    #ifdef DC_SERVER
    if(seq != cs_seq) printf("seq != cs_seq \n");
    
    if (client_id != -1) 
    {
        class PlayerAgent_Snapshot P;
        
        P.id = id;
        P.seq = (cs_seq+1) % 256;

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

        A.id = id;
        A.seq = (cs_seq+1) % 256;

        A.x = s.x;
        A.y = s.y;
        A.z = s.z;
        A.vx = s.vx;
        A.vy = s.vy;
        A.vz = s.vz;

        A.theta = s.theta;
        A.phi = s.phi;

        A.broadcast();

        //clean out old control state
        int i;
        int index;
        for(i=16;i<96;i++)
        {
            index = (seq + i)%128;
            this->cs[index].seq = -1;
        }
    }
    #endif
}

void Agent_state::handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz) {
    state_snapshot.seq = seq;
    state_snapshot.theta = theta;
    state_snapshot.phi = phi;
    state_snapshot.x=x;state_snapshot.y=y;state_snapshot.z=z;
    state_snapshot.vx=vx;state_snapshot.vy=vy;state_snapshot.vz=vz;

    int i;
    int index;
    for(i=16;i<96;i++){
        index = (seq + i)%128;
        cs[index].seq = -1;
    }
                
    state_rollback = state_snapshot; //when new snapshot comes, in, set rollbacks
    cs_seq = seq;

    s = state_snapshot;

    #ifdef DC_CLIENT
    AgentDraw::add_snapshot_to_history(this);
    #endif

    tick();
}

void Agent_state::set_state(float  x, float y, float z, float vx, float vy, float vz) {
    s.x = x;
    s.y = y;
    s.z = z;
    s.vx = vx;
    s.vy = vy;
    s.vz = vz;
}

void Agent_state::set_angles(float theta, float phi) {
    s.theta = theta;
    s.phi = phi;
}

void Agent_state::get_spawn_point(int* spawn) {
    // get spawn area based on team
    int x_max, x_min, y_max, y_min;
    switch (this->status.team)
    {
        case 0:
            x_min = 0;
            x_max = map_dim.x;
            y_min = 0;
            y_max = map_dim.y;
            break;
        case 1:
            x_min = 0;
            x_max = map_dim.x/2;
            y_min = 0;
            y_max = map_dim.y/2;
            break;
        case 2:
            x_min = map_dim.x/2;
            x_max = map_dim.x;
            y_min = map_dim.y/2;
            y_max = map_dim.y;
            break;
        default:
            printf("Agent_state::get_spawn_point, invalid team %d\n", this->status.team);
            spawn[0]=spawn[1]=spawn[2]=0;
            return;
    }
    do {
        spawn[0] = randrange(x_min, x_max-1);
        spawn[1] = randrange(y_min, y_max-1);
        spawn[2] = _get_highest_open_block(spawn[0], spawn[1], (int)(ceil(box.b_height)));
    } while (spawn[2] <= 0);
    printf("Spawning at z_level %d\n", spawn[2]);
}

void Agent_state::spawn_state() {
    // update position
    int spawn[3];
    get_spawn_point(spawn);
    teleport(spawn[0], spawn[1], spawn[2], 0, 0, 0, 0.5f, 0.0f);
}

void Agent_state::init_vox()
{
    this->vox = new Voxel_model(&agent_vox_dat, this->id, this->type);
    this->vox->set_hitscan(true);
    this->vox->register_hitscan();
}

Agent_state::Agent_state(int id)
:
id (id), type(OBJ_TYPE_AGENT), status(this), weapons(this)
#ifdef DC_CLIENT
, event(this)
#endif
{
    set_state(16.5f, 16.5f, 16.5f, 0.0f, 0.0f, 0.0f);
    set_angles(0.5f, 0.0f);

    box.b_height = AGENT_HEIGHT;
    box.c_height = AGENT_HEIGHT_CROUCHED;
    box.box_r = AGENT_BOX_RADIUS;

    cs_seq = 0;

    printf("Agent_state::Agent_state, new agent, id=%i \n", id);
    
    state_snapshot.seq = -1;
    state_rollback.seq = -1;
    for(int i=0; i<128; i++)
    {
        cs[i].seq = -1;
        cs[i].cs = 0;
        cs[i].theta = 0;
        cs[i].phi = 0;
    }

    client_id = id;

    // add to NetServer pool
    NetServer::assign_agent_to_client(this->client_id, this);

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
id(id), type(OBJ_TYPE_AGENT), status(this), weapons(this)
#ifdef DC_CLIENT
, event(this)
#endif
{
    set_state(x, y, z, vx, vy, vz);
    set_angles(0.5f, 0.0f);
    
    box.b_height = AGENT_HEIGHT;
    box.c_height = AGENT_HEIGHT_CROUCHED;
    box.box_r = AGENT_BOX_RADIUS;

    cs_seq = 0;

    printf("Agent_state::Agent_state, new agent, id=%i \n", id);
    
    state_snapshot.seq = -1;
    state_rollback.seq = -1;
    for(int i=0; i<128; i++)
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

Agent_state::~Agent_state() {

    #ifdef DC_SERVER
    agent_destroy_StoC msg;
    msg.id = id;
    msg.broadcast();
    #endif

    #ifdef DC_CLIENT
    if (this->vox != NULL) delete this->vox;
    #endif
}


void Agent_state::revert_to_snapshot() {
    s = state_snapshot;
    cs_seq = state_snapshot.seq;
}

void Agent_state::revert_to_rollback() {
    s = state_rollback;            
    cs_seq = state_rollback.seq;
}

void Agent_state::print_cs()
{
    uint16_t cs = this->cs[this->cs_seq].cs;
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

    printf("f,b,l,r = %d%d%d%d\n", forward, backwards, left, right);
    printf("jet=%d\n", jetpack);
    printf("jump=%d\n", jump);
    printf("crouch=%d\n", crouch);
    printf("boost=%d\n", boost);
    printf("misc123= %d%d%d\n", misc1, misc2, misc3);
}

Agent_control_state Agent_state::get_current_control_state()
{
    return this->cs[(this->cs_seq-1)%128];
}

int Agent_state::crouched()
{
    return this->get_current_control_state().cs & 64;
}

float Agent_state::camera_height() {
    if (this->crouched())
        return CAMERA_HEIGHT_CROUCHED;
    return CAMERA_HEIGHT;
}
