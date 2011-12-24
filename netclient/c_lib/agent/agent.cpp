#include "agent.hpp"

#include <c_lib/agent/net_agent.hpp>
#include <c_lib/agent/agent_draw.hpp>
#include <c_lib/agent/player_agent.hpp>

#include <c_lib/defines.h>
#include <math.h>

void Agent_list::draw() 
{
    #ifdef DC_CLIENT
    int i;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    bool you;
    for(i=0; i<AGENT_MAX; i++) { //max_n
        if (a[i] == NULL) continue;
        you = (a[i]->id == ClientState::playerAgent_state.agent_id);
        if ((first_person && you) ||
            a[i]->status.dead) {
            continue;
        }
        a[i]->draw();
    }
    glEnd();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glColor3ub(255,255,255);

    #endif
}

#include <t_map/t_map.hpp>
#include <t_map/t_properties.h>

    /*
        Collision check may be sped up by caching locally
        May be sped up further by only updating when agent changes cells or terrain changes
    */

static inline int _collision_check(int x, int y, int z) {
    return isActive(_get(x,y,z));
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
    static PlayerAgent_id_StoC msg;
    msg.id = id;
    msg.sendToClient(client_id);
    #endif
}

void Agent_state::teleport(float x,float y,float z) {
    s.x = x;
    s.y = y;
    s.z = z;

    static Agent_teleport_message A;

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
    
    static Agent_teleport_message A;

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

// assume box_r < 1

#include <math.h>

static inline void collision_check1(float box_r, float box_h, float x, float y, float z, int collision[6]) {
    //north +x
    //south -x
    //west +y
    //east -y
    //top +z
    //bottom -z
    int i;
    for(i=0; i<6; i++) collision[i] = 0;

    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int z0 = z;
    int z1 = z+1.0f;
    int z2 = z+box_h;

    //int z_min = s.z;
    //int z_max = s.z + b_height + 1.0;

    //upper left
    //upper right
    //bottom right
    //bottom left
    if(isActive(_get(x_max,y_max,z0) != 0)) {
        //north, west, bottom
        collision[0]++; //north 
        collision[2]++; //west
        collision[5]++; //bottom
    }
    if(isActive(_get(x_max,y_max,z1) != 0)) {
        //north, west
        collision[0]++; //north 
        collision[2]++; //west
    }
    if(isActive(_get(x_max,y_max,z2) != 0)) {
        //north, west, top
        collision[0]++; //north 
        collision[2]++; //west
        collision[4]++; //top     
    }


    if(isActive(_get(x_max,y_min,z0) != 0)) {
        //north, east, bottom
        collision[0]++; //north 
        collision[3]++; //east 
        collision[5]++; //bottom
    }
    if(isActive(_get(x_max,y_min,z1) != 0)) {
        //north, east
        collision[0]++; //north 
        collision[3]++; //east 
    }
    if(isActive(_get(x_max,y_min,z2) != 0)) {
        //north, east, top
        collision[0]++; //north 
        collision[3]++; //east
        collision[4]++; //top
    }


    if(isActive(_get(x_min,y_min,z0) != 0)) {
        //south, east, bottom
        collision[1]++; //south
        collision[3]++; //east
        collision[5]++; //bottom
    }
    if(isActive(_get(x_min,y_min,z1) != 0)) {
        //south, east
        collision[1]++; //south
        collision[3]++; //east 
    }
    if(isActive(_get(x_min,y_min,z2) != 0)) {
        //south, east, top
        collision[1]++; //south
        collision[3]++; //east 
        collision[4]++; //top
    }

    if(isActive(_get(x_min,y_max,z0) != 0)) {
        //south, west
        collision[1]++; //south
        collision[2]++; //west
        collision[5]++; //bottom
    }
    if(isActive(_get(x_min,y_max,z1) != 0)) {
        //south, west
        collision[1]++; //south
        collision[2]++; //west
    }
    if(isActive(_get(x_min,y_max,z2) != 0)) {
        //south, west
        collision[1]++; //south
        collision[2]++; //west
        collision[4]++; //top
    }

    //printf("collision: n=%i, s=%i, w=%i, e=%i, t=%i, b=%i \n", collision[0],collision[1],collision[2],collision[3],collision[4],collision[5] );
}

static inline bool collision_check2(float box_r, float box_h, float x, float y, float z) {
    //north +x
    //south -x
    //west +y
    //east -y
    //top +z
    //bottom -z

    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int z0 = z;
    int z1 = z + 1.0f;
    int z2 = z + box_h;

    //int z_min = s.z;
    //int z_max = s.z + b_height + 1.0;

    //upper left
    //upper right
    //bottom right
    //bottom left
    if(isActive(_get(x_max,y_max,z0) != 0) || isActive(_get(x_max,y_max,z1) != 0) || isActive(_get(x_max,y_max,z2) != 0) ) {
        //north, west
        return true;
    }


    if(isActive(_get(x_max,y_min,z0) != 0) || isActive(_get(x_max,y_min,z1) != 0) || isActive(_get(x_max,y_min,z2) != 0) ) {
        //north, east
        return true;
    }

    if(isActive(_get(x_min,y_min,z0) != 0) || isActive(_get(x_min,y_min,z1) != 0) || isActive(_get(x_min,y_min,z2) != 0) ) {
        //south, east
        return true;
    }

    if(isActive(_get(x_min,y_max,z0) != 0) || isActive(_get(x_min,y_max,z1) != 0) || isActive(_get(x_min,y_max,z2) != 0) ) {
        //south, west
        return true;
    }

    return false;
}

// for when box_h < 1
static inline bool collision_check_short(float box_r, float box_h, float x, float y, float z) {
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int z0 = z;
    int z1 = z + box_h;

    if(isActive(_get(x_max,y_max,z0) != 0) || isActive(_get(x_max,y_max,z1) != 0)) {
        //north, west
        return true;
    }

    if(isActive(_get(x_max,y_min,z0) != 0) || isActive(_get(x_max,y_min,z1) != 0)) {
        //north, east
        return true;
    }

    if(isActive(_get(x_min,y_min,z0) != 0) || isActive(_get(x_min,y_min,z1) != 0)) {
        //south, east
        return true;
    }

    if(isActive(_get(x_min,y_max,z0) != 0) || isActive(_get(x_min,y_max,z1) != 0)) {
        //south, west
        return true;
    }

    return false;
}

#define GROUND_MARGIN 0.10f
// checks the (agent bottom - margin) at 4 corners of the agent
static inline bool on_ground(float box_r, float x, float y, float z) {

    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int z0 = z - GROUND_MARGIN;

    //upper left
    //upper right
    //bottom right
    //bottom left

    //in future, check tile height if collision
    if(isActive(_get(x_max,y_max,z0))) {
        return true;
    }

    if(isActive(_get(x_max,y_min,z0))) {
        return true; 
    }

    if(isActive(_get(x_min,y_min,z0))) {
        return true;
    }

    if(isActive(_get(x_min,y_max,z0))) {
        return true;
    }

    return false;
}

static inline bool on_solid_ground(float box_r, float x, float y, float z) {
    int x_min = (int)(x - box_r);
    int x_max = (int)(x + box_r);

    int y_min = (int)(y - box_r);
    int y_max = (int)(y + box_r);

    int z0 = (int)(z - GROUND_MARGIN);

    //upper left
    //upper right
    //bottom right
    //bottom left

    //in future, check tile height if collision
    if(isSolid(_get(x_max,y_max,z0))) {
        return true;
    }

    if(isSolid(_get(x_max,y_min,z0))) {
        return true; 
    }

    if(isSolid(_get(x_min,y_min,z0))) {
        return true;
    }

    if(isSolid(_get(x_min,y_max,z0))) {
        return true;
    }

    return false;
}

inline bool can_stand_up_inner(int x_min, int x_max, int y_min, int y_max, float z) {
    int z0 = (int)(z);

    if (isActive(_get(x_min, y_min, z0)) ||
        isActive(_get(x_max, y_min, z0)) ||
        isActive(_get(x_min, y_max, z0)) ||
        isActive(_get(x_max, y_max, z0))) {
        return false;
    }
    return true;
}

// check if there will be a collision if standing up
inline bool can_stand_up(float box_r, float x, float y, float z, float current_h, float new_h) {
    if (new_h <= current_h) printf("can_stand_up:: new height <= current height. You're doing it wrong\n");
    int x_min = (int)(x - box_r);
    int x_max = (int)(x + box_r);

    int y_min = (int)(y - box_r);
    int y_max = (int)(y + box_r);    

    new_h += z;

    z += current_h;

    bool yes = true;

    yes = can_stand_up_inner(x_min, x_max, y_min, y_max, z);
    if (yes) {
        do {
            z += 1.0f;
            z = (z > new_h) ? (new_h + 0.001f) : z;
            yes = can_stand_up_inner(x_min, x_max, y_min, y_max, z);
        } while (yes && z <= new_h);
    }
    return yes;
}

void Agent_state::_tick() 
{
    //printf("Agent_state._tick: processing cs_seq= %i, index== %i \n",cs_seq, index);

    int _tc =0;
    //int index = (cs_seq+1) % 128;

    struct Agent_control_state _cs;

    while(cs[(cs_seq+1) % 128].seq == (cs_seq+1)% 256) {

        //int index = (cs_seq+1) % 128;
        cs_seq = (cs_seq+1)%256;
        _cs = cs[cs_seq % 128];

        s = _agent_tick(_cs, box, s);
    //s.tick(_cs, box);

        _tc++;
    }

    status.respawn();
}


//takes an agent state and control state and returns new agent state
inline class AgentState _agent_tick(const struct Agent_control_state _cs, const struct Agent_collision_box box, class AgentState as)
//void AgentState::tick(const struct Agent_control_state _cs, const struct Agent_collision_box box)
 {

    /*    
        int forward =0;
        int backwards =0;
        int left =0;
        int right =0;
        int jetpack =0;
    */  

    int a_cs = _cs.cs;
    //set control state variables
    //printf("cs= %i \n", a_cs);
    bool forward     = a_cs & 1? 1 :0;
    bool backwards   = a_cs & 2? 1 :0;
    bool left        = a_cs & 4? 1 :0;
    bool right       = a_cs & 8? 1 :0;
    bool jetpack     = a_cs & 16? 1 :0;
    bool jump        = a_cs & 32? 1 :0;
    //implemented, but unused
    /*
    bool crouch      = a_cs & 64? 1 :0;
    bool boost       = a_cs & 128? 1 :0;
    bool misc1       = a_cs & 256? 1 :0;
    bool misc2       = a_cs & 512? 1 :0;
    bool misc3       = a_cs & 1024? 1 :0;     
    */
/*
    //local cordinates
    int l_x = as.x;
    int l_y = as.y;
    int l_z = as.z;

    //local float cordinates
    float fl_x = as.x - floor(as.x);
    float fl_y = as.y - floor(as.y);
    float fl_z = as.z - floor(as.z);
*/

    const float tr = 10.0f;    //tick rate
    const float tr2 = tr*tr;

    const float xy_speed = 2.0f / tr;
    const float z_gravity = -2.0f / tr2;
    const float z_jetpack = (0.5f / tr2) - z_gravity;

    const float jump_boost = 0.25f;

    //const float ground_distance = 0.02;   // unused
    const float z_bounce = 0.20f;
    const float z_bounce_v_threshold = 1.5f / tr;

    const float pi = 3.14159265f;

    //int collision[6];
    //north +as.x
    //south -as.x
    //west +as.y
    //east -as.y
    //top +as.z
    //bottom -as.z
    //collision_check1(box.box_r, box.b_height, as.x,as.y,as.z, collision);

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
    else // under the floor, go back up
    {
        as.vz -= z_gravity;
    }    
    
    if (jetpack) {
        as.vz += z_jetpack;
    }

    /*
    // jump
    if (jump && as.jump_ready) {
        as.vz += jump_boost;
    }
    */
    if (jump) {
        as.vz += jump_boost;
    }
     
    float new_x, new_y, new_z;
    new_x = as.x + as.vx + cs_vx;
    new_y = as.y + as.vy + cs_vy;
    new_z = as.z + as.vz;

    //using function pointer may throw off 

    /*
        Warning: using function pointer may throw off brach prediction and hurt performance, look this up
    */
/*
    //crouching
    float height;
    bool (*collision_check)(float, float, float, float, float);
    if (crouch || (as.crouching && !can_stand_up(box.box_r, as.x, as.y, as.z, box.c_height, box.b_height))) {
        as.crouching = true;
        height = box.c_height;
        collision_check = &collision_check_short;
    } else {
        as.crouching = false;
        height = box.b_height;
        collision_check = &collision_check2;
    }
*/
    float height;
    height = box.b_height;
    bool (*collision_check)(float, float, float, float, float);
    collision_check = &collision_check2;

    // collision
    bool current_collision = collision_check(box.box_r, height, as.x,as.y,as.z);
    if(current_collision) {
        as.x = new_x;
        as.y = new_y;
        as.z += 0.02f; //nudge factor
        if(as.vz < 0.0f) as.vz = 0.0f;

        //printf("Agent Tick: warning current collision is true!\n");
        return as;
    }

    /*
        Collision Order: as.x,as.y,as.z
    */
    bool collision_x = collision_check(box.box_r, height, new_x,as.y,as.z);
    if(collision_x) {
        new_x = as.x;
        as.vx = 0.0f;
    }

    bool collision_y = collision_check(box.box_r, height, new_x,new_y,as.z);
    if(collision_y) {
        new_y = as.y;
        as.vy = 0.0f;
    }

    //top and bottom matter
    bool collision_z = collision_check(box.box_r, height, new_x,new_y,new_z);
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

    // allow jumping if on ground
    
    //bool is_on_ground = on_solid_ground(box.box_r, as.x, as.y, as.z);

    /*
    if (! is_on_ground) {
        as.jump_ready = false;
    } else {
        as.jump_ready = true;
    }

    // or under the floor
    if (as.z < 0.0f) {
        as.jump_ready = true;
    }
    */

    as.theta = _cs.theta;
    as.phi = _cs.phi;

    return as;
}

    /*
        int id;
        int seq;
        int tick;

        float x;
        float y;
        float z;
        float vx,vy,vz;
        float theta, phi;
    */

void Agent_state::handle_control_state(int _seq, int _cs, float _theta, float _phi) {
    //printf("control state received: agent=%i, seq=%i, cs=%i \n", id, _seq, _cs);
    int index = _seq%128;

    cs[index].seq = _seq;
    cs[index].cs = _cs;
    cs[index].theta = _theta;
    cs[index].phi = _phi;

    //printf("cs_seq= %i, _seq= %i \n", cs_seq, _seq);

    _tick();

    #ifdef DC_SERVER
    if(_seq != cs_seq) {
        
        printf("_seq != cs_seq \n");
    }
    
    #endif

    #ifdef DC_SERVER
    
        if(client_id != -1) 
        {
            PlayerAgent_Snapshot P;
            
            P.id = id;
            P.seq = cs_seq;

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

        if( _seq % 32 == 0 ) {
            Agent_state_message A;

            A.id = id;
            A.seq = cs_seq;

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
            for(i=16;i<96;i++){
                index = (_seq + i)%128;
                cs[index].seq = -1;
            }
        }
    #endif
    //printf("control state= %i\n", new_control_state);
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
    //cs_window_min = seq;
    //printf("handle_state_snapshot: seq= %i, cs_seq= %i \n", seq, cs_seq);
    cs_seq = seq;

    s = state_snapshot;

    #ifdef DC_CLIENT
    AgentDraw::add_snapshot_to_history(this);
    #endif

    //if (tick_mode == use_jetpack) _tick_jetpack();
    //else if (tick_mode == use_jump) _tick_jump();
    _tick();
}

void Agent_state::set_state(float  _x, float _y, float _z, float _vx, float _vy, float _vz) {
    s.x = _x;
    s.y = _y;
    s.z = _z;
    s.vx = _vx;
    s.vy = _vy;
    s.vz = _vz;
}

void Agent_state::set_angles(float theta, float phi) {
    s.theta = theta;
    s.phi = phi;
}

Agent_state::Agent_state(int id) : id (id), status(this)
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

    owner = 0;

    printf("Agent_state::Agent_state, new agent, id=%i \n", id);

    _new_control_state = 0;
    
    tick_n = 0; //increment when ticking
    ctick = 0;  //increment when control state received

    state_snapshot.seq = -1;
    state_rollback.seq = -1;
    int i;
    for(i=0; i<128;i++) cs[i].seq = -1;

    client_id = -1;

    #ifdef DC_SERVER
    static agent_create_StoC msg;
    msg.id = id;
    msg.owner = owner;
    msg.broadcast();
    #endif

    #ifdef DC_CLIENT
    vox = new Agent_vox();
    #endif
}

Agent_state::Agent_state(int id, int owner, float x, float y, float z, float vx, float vy, float vz)
: id(id), owner(owner), status(this)
#ifdef DC_CLIENT
, event(this)
#endif
{
    set_state(x, y, z, vx, vy, vz);
    set_angles(0.5f, 0.0f);
    
    box.b_height = AGENT_HEIGHT;
    box.box_r = AGENT_BOX_RADIUS;

    cs_seq = 0;

    printf("Agent_state::Agent_state, new agent, id=%i \n", id);

    _new_control_state = 0;
    
    tick_n = 0; //increment when ticking
    ctick = 0;  //increment when control state received

    state_snapshot.seq = -1;
    state_rollback.seq = -1;
    int i;
    for(i=0; i<128;i++) cs[i].seq = -1;

    client_id = -1;

    #ifdef DC_SERVER
    static agent_create_StoC msg;
    msg.id = id;
    msg.owner = owner;
    msg.broadcast();
    #endif

    #ifdef DC_CLIENT
    vox = new Agent_vox();
    #endif
}

void Agent_state::draw() {
#ifdef DC_CLIENT
    if (vox != NULL) {
        vox->draw(s.x, s.y, s.z, s.theta, s.phi);
    }
#endif
}

void Agent_state::client_tick() {
    //_tick();
    //tick on input received
    return;
}

void Agent_state::server_tick() {
    return;
}

void Agent_list::send_to_client(int client_id) {
    int i;
    agent_create_StoC* msg;
    for (i=1; i<AGENT_MAX; i++) {   // start at 1, 0-agent shouldnt be sent
        if (a[i]==NULL) continue;
        msg = new agent_create_StoC(a[i]->id, a[i]->owner);
        msg->sendToClient(client_id);
    }
}

