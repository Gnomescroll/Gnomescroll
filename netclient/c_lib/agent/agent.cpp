#include "agent.hpp"

#include <c_lib/agent/net_agent.cpp>
#include <c_lib/agent/agent_draw.hpp>
#include <c_lib/defines.h>
#include <math.h>

#ifdef DC_CLIENT
static int n_agents_to_draw = 0;
static int agents_to_draw[AGENT_MAX];
#endif

// deprecate
static enum tick_modes tick_mode = use_jetpack;
void set_agent_tick_mode(int mode) {
    if (mode == use_jetpack) {
        tick_mode = use_jetpack;
    }
    else if (mode == use_jump) {
        tick_mode = use_jump;
    }
    else {
        printf("set_agent_tick_mode :: Unrecognized mode %d\n", mode);
    }
}

// default draw mode, uses agents_to_draw list
void Agent_list::draw() 
{
    #ifdef DC_CLIENT
    int i,j;

    glDisable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glBegin(GL_QUADS);
    //glEnable(GL_CULL_FACE);   // for bounding box lines (disabled)

    for(i=0; i<n_agents_to_draw; i++) { //max_n
        j = agents_to_draw[i];
        if (j < 0) continue;
        if(a[j] != NULL) {
          a[j]->draw();
        }
    }

    //glDisable(GL_CULL_FACE);
    glDisable (GL_DEPTH_TEST);
    glEnd();
    #endif
}

void Agent_list::draw(int all) 
{
    #ifdef DC_CLIENT
        if (! all) return;
        int i;

    glDisable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glBegin(GL_QUADS);
    //glEnable(GL_CULL_FACE);   // for bounding box lines (disabled)

    for(i=0; i<n_max; i++) { //max_n
        if(a[i] != NULL) {
            printf("SHOULD DRAW AGENT %d\n", i);
            a[i]->draw();
        }
    }

    //glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnd();
    #endif
}


    /* 
        if( a_cs & 1 ) {
            //forward
            //printf("Agent_state._tick: agent forward \n");
            s.x += 0.10;
            forward = 1;
        }
        if( a_cs & 2 ) {
            //backward
            //printf("Agent_state._tick: agent backward \n");
            s.x -= 0.10;
            backwards = 1;
        }
        if( a_cs & 4 ) {
            //left
            //printf("Agent_state._tick: agent left \n");
            s.y += 0.10;
            left = 1;
        }
        if( a_cs & 8 ) {
            //right
            //printf("Agent_state._tick: agent right \n");
            s.y -= 0.10;
            right = 1;
        }
        if( a_cs & 16 ) {
            //jet
            s.z += 0.01;
            jet = 1;
        }  
    */

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

    /*
        Collision check may be sped up by caching locally
        May be sped up further by only updating when agent changes cells or terrain changes
    */

static inline int _collision_check(int x, int y, int z) {
    return isActive(_get(x,y,z));
}

void Agent_state::teleport(float x,float y,float z) {
    s.x = x;
    s.y = y;
    s.z = z;

    Agent_state_message A;

    A.id = id;
    A.seq = cs_seq;

    A.x = s.x;
    A.y = s.y;
    A.z = s.z;
    A.vx = s.vx;
    A.vy = s.vy;
    A.vz = s.vz;
    A.broadcast();
}

// assume box_r < 1

#include <math.h>

inline void collision_check1(float box_r, float box_h, float x, float y, float z, int collision[6]) {
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

inline bool collision_check2(float box_r, float box_h, float x, float y, float z) {
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
inline bool collision_check_short(float box_r, float box_h, float x, float y, float z) {
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
inline bool on_ground(float box_r, float x, float y, float z) {

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

inline bool on_solid_ground(float box_r, float x, float y, float z) {
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
    int x_min = (int)(x - box_r);
    int x_max = (int)(x + box_r);

    int y_min = (int)(y - box_r);
    int y_max = (int)(y + box_r);    

    z += current_h;

    bool yes = true;

    do {
        yes = can_stand_up_inner(x_min, x_max, y_min, y_max, z);
        z += 1.0f;
        z = (z > new_h) ? new_h : z;
    } while (yes && z < new_h);

    return yes;
}


void Agent_state::save_state() {
    // copies current state to old state
    s_old.seq = s.seq;
    s_old.phi = s.phi;
    s_old.theta = s.theta;
    s_old.x = s.x;
    s_old.y = s.y;
    s_old.z = s.z;
    s_old.vx = s.vx;
    s_old.vy = s.vy;
    s_old.vz = s.vz;
}

void Agent_state::_tick()
 {

    save_state();

    //printf("Agent_state._tick: processing cs_seq= %i, index== %i \n",cs_seq, index);

    int index = (cs_seq+1) % 128;

    int _tc =0;
    while(cs[(cs_seq+1) % 128].seq == (cs_seq+1)% 256) {
        _tc++;
        cs_seq = (cs_seq+1)%256;

        s.theta = cs[index].theta;
        s.phi = cs[index].phi;

    /*    
        int forward =0;
        int backwards =0;
        int left =0;
        int right =0;
        int jetpack =0;
    */  
        int a_cs = cs[index].cs;

        //set control state variables
        //printf("cs= %i \n", a_cs);
        bool forward     = a_cs & 1? 1 :0;
        bool backwards   = a_cs & 2? 1 :0;
        bool left        = a_cs & 4? 1 :0;
        bool right       = a_cs & 8? 1 :0;
        bool jetpack     = a_cs & 16? 1 :0;
        bool jump        = a_cs & 32? 1 :0;
        bool crouch      = a_cs & 64? 1 :0;
        bool boost       = a_cs & 128? 1 :0;
        bool misc1       = a_cs & 256? 1 :0;
        bool misc2       = a_cs & 512? 1 :0;
        bool misc3       = a_cs & 1024? 1 :0;     

    /*
        //local cordinates
        int l_x = s.x;
        int l_y = s.y;
        int l_z = s.z;

        //local float cordinates
        float fl_x = s.x - floor(s.x);
        float fl_y = s.y - floor(s.y);
        float fl_z = s.z - floor(s.z);
    */

        const float tr = 10.0f;    //tick rate
        const float tr2 = tr*tr;

        const float xy_speed = 2.0f / tr;
        const float z_gravity = -2.0f / tr2;
        const float z_jetpack = (0.5f / tr2) - z_gravity;

        float jump_boost = 0.25f;

        //const float ground_distance = 0.02;   // unused
        const float z_bounce = 0.20f;
        const float z_bounce_v_threshold = 1.5f / tr;

        const float pi = 3.14159265f;

        //int collision[6];
        //north +x
        //south -x
        //west +y
        //east -y
        //top +z
        //bottom -z
        //collision_check1(box_r, b_height, s.x,s.y,s.z, collision);

        float cs_vx = 0;
        float cs_vy = 0;

        if(forward)
        {
                cs_vx += xy_speed*cos( s.theta * pi);
                cs_vy += xy_speed*sin( s.theta * pi);
        }
        if(backwards)
        {
                cs_vx += -xy_speed*cos( s.theta * pi);
                cs_vy += -xy_speed*sin( s.theta * pi);
        }
        if(left) 
        {
                cs_vx += xy_speed*cos( s.theta * pi + pi/2);
                cs_vy += xy_speed*sin( s.theta * pi + pi/2);
        }
        if(right) 
        {
                cs_vx += -xy_speed*cos( s.theta * pi + pi/2);
                cs_vy += -xy_speed*sin( s.theta * pi + pi/2);
        }

        //jet pack and gravity
        if(s.z > 0)
        {
            s.vz += z_gravity;
        } 
        else // under the floor, go back up
        {
            s.vz -= z_gravity;
        }    
        
        if (jetpack) {
            s.vz += z_jetpack;
        }

        // jump
        if (jump) {
            if (!jump_held && jump_ready) {
                s.vz += jump_boost;
            }
            jump_held = true;
        } else {
            jump_held = false;
        }

        float new_x, new_y, new_z;
        new_x = s.x + s.vx + cs_vx;
        new_y = s.y + s.vy + cs_vy;
        new_z = s.z + s.vz;

        bool (*collision_check)(float, float, float, float, float);
        if (crouch || (crouching && !can_stand_up(box_r, s.x, s.y, s.z, AGENT_HEIGHT, b_height))) {
            crouching = true;
            b_height = AGENT_HEIGHT_CROUCHED;
            camera_height = AGENT_CAMERA_HEIGHT_CROUCHED;
            collision_check = &collision_check_short;
        } else {
            crouching = false;
            b_height = AGENT_HEIGHT;
            camera_height = AGENT_CAMERA_HEIGHT;
            collision_check = &collision_check2;
        }

        bool current_collision = collision_check(box_r, b_height, s.x,s.y,s.z);
        if(current_collision) {
            s.x = new_x;
            s.y = new_y;
            s.z += 0.02; //nudge factor
            if(s.vz < 0) s.vz = 0;
            continue;
        }

        /*
            Collision Order: x,y,z
        */
        bool collision_x = collision_check(box_r, b_height, new_x,s.y,s.z);
        if(collision_x) {
            new_x = s.x;
            s.vx = 0;
        }

        bool collision_y = collision_check(box_r, b_height, new_x,new_y,s.z);
        if(collision_y) {
            new_y = s.y;
            s.vy = 0;
        }

        //top and bottom matter
        bool collision_z = collision_check(box_r, b_height, new_x,new_y,new_z);
        if(collision_z) {

            if(s.vz < -z_bounce_v_threshold)
            {
                s.vz *= -1 *z_bounce;
            }
            else
            {
                s.vz = 0;
            }

            new_z = s.z + s.vz;
        }       

        // allow jumping if on ground
        bool is_on_ground = on_solid_ground(box_r, s.x, s.y, s.z);
        if (! is_on_ground) {
            jump_ready = false;
        } else {
            jump_ready = true;
        }

        // or under the floor
        if (s.z < 0.0f) {
            jump_ready = true;
        }

        s.x = new_x;
        s.y = new_y;
        s.z = new_z;

    }
}

void Agent_state::handle_control_state(int _seq, int _cs, float _theta, float _phi) {
    //printf("control state received: agent=%i, seq=%i, cs=%i \n", id, _seq, _cs);
    int index = _seq%128;

    cs[index].seq = _seq;
    cs[index].cs = _cs;
    cs[index].theta = _theta;
    cs[index].phi = _phi;

    //printf("cs_seq= %i, _seq= %i \n", cs_seq, _seq);

    #ifdef DC_SERVER
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
    //if (tick_mode == use_jetpack) _tick_jetpack();
    //else if (tick_mode == use_jump) _tick_jump();
    _tick();
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
    cs_window_min = seq;
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

Agent_state::Agent_state(int _id) {
    id = _id;

    set_state(16.5f, 16.5f, 16.5f, 0.0f, 0.0f, 0.0f);

    b_height = AGENT_HEIGHT;
    box_r = AGENT_BOX_RADIUS;
    camera_height = AGENT_CAMERA_HEIGHT;

    jump_ready = true;
    crouching = false;

    cs_seq = 0;

    printf("Agent_state::Agent_state, new agent, id=%i \n", id);

    _new_control_state = 0;
    
    tick_n = 0; //increment when ticking
    ctick = 0;  //increment when control state received
    theta = 0.0;
    phi = 0.0;

    state_snapshot.seq = -1;
    state_rollback.seq = -1;
    int i;
    for(i=0; i<128;i++) cs[i].seq = -1;

    #ifdef DC_CLIENT
    vox = new Agent_vox();
    #endif
}

Agent_state::Agent_state(int _id, float _x, float _y, float _z, float _vx, float _vy, float _vz) {
    id = _id;

    set_state(_x, _y, _z, _vx, _vy, _vz);

    b_height = AGENT_HEIGHT;
    box_r = AGENT_BOX_RADIUS;
    camera_height = AGENT_CAMERA_HEIGHT;

    jump_ready = true;
    crouching = false;

    //s.x = 16.5;
    //s.y = 16.5;

    cs_seq = 0;

    printf("Agent_state::Agent_state, new agent, id=%i \n", id);

    _new_control_state = 0;
    
    tick_n = 0; //increment when ticking
    ctick = 0;  //increment when control state received
    theta = 0.0;
    phi = 0.0;

    state_snapshot.seq = -1;
    state_rollback.seq = -1;
    int i;
    for(i=0; i<128;i++) cs[i].seq = -1;

    #ifdef DC_CLIENT
    vox = new Agent_vox();
    #endif
}

void Agent_state::draw() {
#ifdef DC_CLIENT
    //AgentDraw::draw_agent(this);
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

void Agent_state::set_interpolated(int t) {
    float weight = (float) t / 30.0f;
    interpolate.x = weight * (s.x - s_old.x) + s_old.x;
    interpolate.y = weight * (s.y - s_old.y) + s_old.y;
    interpolate.z = weight * (s.z - s_old.z) + s_old.z;
}



#ifdef DC_CLIENT
void init_agent_vox_part(int id, int part, unsigned short vox_x, unsigned short vox_y, unsigned short vox_z, float vox_size) {
    Agent_state* s = ClientState::agent_list.get(id);
    if (s==NULL || s->vox == NULL) return;
    s->vox->init_vox_part(part, vox_x, vox_y, vox_z, vox_size);
}

void init_agent_vox_done(int id) {
    Agent_state* s = ClientState::agent_list.get(id);
    if (s==NULL || s->vox == NULL) return;
    s->vox->init_vox_done();
}

void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a) {
    Agent_state* s = ClientState::agent_list.get(id);
    if (s==NULL || s->vox == NULL) return;
    s->vox->set_vox_volume(part, x,y,z, r,g,b,a);
}

void set_agent_limb_direction(int id, int part, float fx, float fy, float fz, float nx, float ny, float nz) {
    Agent_state* s = ClientState::agent_list.get(id);
    if (s==NULL || s->vox == NULL) return;
    s->vox->set_limb_direction(part, fx,fy,fz, nx,ny,nz);
}

void set_agent_limb_anchor_point(int id, int part, float length, float ax, float ay, float az) {
    Agent_state* s = ClientState::agent_list.get(id);
    if (s==NULL || s->vox == NULL) return;
    s->vox->set_limb_base_anchor_point(part, length, ax,ay,az);
}

void init_agents_to_draw() {
    int i;
    for (i=0; i<AGENT_MAX; i++) {
        agents_to_draw[i] = -1;
    }
    n_agents_to_draw = 0;
}

void clear_agents_to_draw() {
    int i;
    for (i=0; i<n_agents_to_draw; i++) {
        agents_to_draw[i] = -1;
    }
    n_agents_to_draw = 0;
}

void set_agents_to_draw(int* ids, int ct) {
    int i;
    for (i=0; i<ct; i++) {
        agents_to_draw[i] = ids[i];
    }
    n_agents_to_draw = ct;
}

#endif



