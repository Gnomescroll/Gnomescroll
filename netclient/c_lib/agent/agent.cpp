
#include "agent.hpp"

#include <c_lib/agent/net_agent.cpp>

#include <c_lib/agent/agent_draw.hpp>

#include <math.h>

void Agent_list::draw() 
{
    #ifdef DC_CLIENT
        int i;

        glDisable(GL_TEXTURE_2D);
        //glEnable(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        for(i=0; i<n_max; i++) { //max_n
            if(a[i] != NULL) {
                a[i]->draw();
            }
        }
        glDisable(GL_CULL_FACE);
        //glEnable(GL_TEXTURE_2D);
        //glDisable(GL_DEPTH_TEST);
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

    //int z_min = s.z;
    //int z_max = s.z + b_height + 1.0;

    //upper left
    //upper right
    //bottom right
    //bottom left
    if(isActive(_get(x_max,y_max,z0) != 0)) {
        //north, west
        collision[0]++; //north 
        collision[2]++; //west
    }

    if(isActive(_get(x_max,y_min,z0) != 0)) {
        //north, east
        collision[0]++; //north 
        collision[3]++; //east 
    }

    if(isActive(_get(x_min,y_min,z0) != 0)) {
        //south, east
        collision[1]++; //south
        collision[3]++; //east 
    }

    if(isActive(_get(x_min,y_max,z0) != 0)) {
        //south, west
        collision[1]++; //south
        collision[2]++; //west
    }

    printf("collision: n=%i, s=%i, w=%i, e=%i, t=%i, b=%i \n", collision[0],collision[1],collision[2],collision[3],collision[4],collision[5] );
}


inline void on_ground() {
    

}

void Agent_state::_tick() {

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

        const float tr = 10.0;    //tick rate
        const float tr2 = tr*tr;

        const float xy_speed = 2.00 / tr;
        const float z_jetpack = 0.80 / tr2;
        const float z_gravity = -.40 / tr2;

        const float ground_distance = 0.02;
        const float z_bounce = 0.65;
        const float z_bounce_v_threshold = 0.35 / tr;

        const float pi = 3.14159265;

        //box properties
        float b_height = 2.5;  //agent collision box height
        float box_r = 0.4;

        int collision[6];
        //north +x
        //south -x
        //west +y
        //east -y
        //top +z
        //bottom -z
        collision_check1(box_r, b_height, s.x,s.y,s.z, collision);

        float cs_vx =0 ;
        float cs_vy =0 ;

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
    /*
        int bx_pos_projected = s.x+s.vx+cs_vx+box_r;    //floor
        int bx_neg_projected = s.x+s.vx+cs_vy-box_r;

        int by_pos_projected = s.y+s.vy+cs_vx+box_r;
        int by_neg_projected = s.y+s.vy+cs_vy-box_r; 
                  
        int bz_pos_projected = s.z+s.vz+cs_vx+b_height;
        int bz_neg_projected = s.z+s.vz;


        //handle x collisions
        if(xc_pos_current == 0 && xc_neg_current == 0)
        {
            if(xc_pos_projected != 0) s.vx = 0.0;
            if(xc_neg_projected != 0) s.vx = 0.0;
        }

        //handle y collisions

        if(yc_pos_current ==0 && yc_neg_current ==0)
        {
            if(yc_pos_projected != 0) s.vy = 0;
            if(yc_neg_projected != 0) s.vy = 0;
        }

        //handle z collision

        //#Hard collision predicted and not inside of something already
        if(zc_neg_projected != 0 && zc_neg_current == 0)
        {
            if(s.vz < 0)
            {
                if(s.vz < -z_bounce_v_threshold)
                {
                    s.vz *= -1 *z_bounce;
                }
                else
                {
                    s.vz = 0;
                }
            }
        }

        if(zc_neg_current != 0) // #if agent is inside of block, float them out the top
        {
            s.z += 0.50 / tr;
        }

        if(zc_pos_current != 0) // #if agent is inside of block, float them out the top
        {
            s.z += -0.50 / tr;
        }

        //jetpack handling
        if(!on_ground) {
            if(s.z>0)
            {
                s.vz += z_gravity;
            } 
            else 
            {
                s.vz -= z_gravity;
            }    
        }
        if(jetpack) {
            s.vz += z_jetpack;
        }
        */

        //newton intergrate positions
        s.x += s.vx + cs_vx;    //use better intergrator
        s.y += s.vy + cs_vy;
        s.z += s.vz;

        s.z = 1.01;
    /*
        printf("cs_vx= %f, cs_vy= %f \n", cs_vx, cs_vy);
        printf("dx= %f, dy= %f, dz= %f \n", s.vx + cs_vx, s.vy + cs_vy, s.vz);
        printf("cs_seq= %i, x= %f, y= %f, z= %f, vx= %f, vy= %f, vz= %f \n",cs_seq, s.x, s.y, s.z, s.vx, s.vy, s.vz);
    */
        //printf("vx= %f, vy= %f, vz= %f \n", s.vx, s.vy, s.vz);
    } //end physics loop
    //printf("_tick: processed %i agent ticks\n", _tc);
    if(id == 0) 
    {
        //printf("x= %f, y= %f, z= %f \n", s.x, s.y, s.z);
        //printf("vx= %f, vy= %f, vz= %f \n", s.vx, s.vy, s.vz);
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
    _tick();
}
Agent_state::Agent_state(int _id) {
    id = _id;
    s.x = 16.5;
    s.y = 16.5;
    s.z = 16;
    s.vx = 0;
    s.vy = 0;
    s.vz = 0;

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
    AgentDraw::draw_agent(this);
#endif
}

void Agent_state::client_tick() {
        //_tick();
        //tick on input received    
    }

void Agent_state::server_tick() {

    return;
}


void agents_draw() {
    #ifdef DC_CLIENT
    ClientState::agent_list.draw();
    #endif
}

int agent_create(int id, float x, float y, float z) {
#ifdef DC_CLIENT
    Agent_state* s = ClientState::agent_list.create(id);
    if (s != NULL) {
        id = s->id;
        s->s.x = x;
        s->s.y = y;
        s->s.z = z;
    }
    
    return id;
#else
    return 0;
#endif
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
    s->vox->set_limb_anchor_point(part, length, ax,ay,az);
}

#endif

void agents_tick() {
    
}



/*

REFERENCE

*/

//tick stuff

/*
    int index = (cs_seq+1) % 128;

    int _tc =0;
    while(cs[(cs_seq+1) % 128].seq == (cs_seq+1)% 256) {
        _tc++;
        cs_seq = (cs_seq+1)%256;

        s.theta = cs[index].theta;
        s.phi = cs[index].phi;


        int a_cs = cs[index].cs;

        //set control state variables
        //printf("cs= %i \n", a_cs);
        bool forward     = a_cs & 1? 1 :0;
        bool backwards   = a_cs & 2? 1 :0;
        bool left        = a_cs & 4? 1 :0;
        bool right       = a_cs & 8? 1 :0;
        bool jetpack     = a_cs & 16? 1 :0;


        const float tr = 10.0;    //tick rate
        const float tr2 = tr*tr;

        const float xy_speed = 2.00 / tr;
        const float z_jetpack = 0.80 / tr2;
        const float z_gravity = -.40 / tr2;

        const float ground_distance = 0.02;
        const float z_bounce = 0.65;
        const float z_bounce_v_threshold = 0.35 / tr;

        const float pi = 3.14159265;

        //box properties
        float b_height = 2.5;  //agent collision box height
        float box_r = 0.4;

//### Collisions on X axis


    //  integer cordinates used for checking voxels for collision

        int bx_pos_current = s.x+box_r;          //floor
        int bx_neg_current = s.x-box_r;

        int by_pos_current = s.y+box_r;
        int by_neg_current = s.y-box_r;

        int bz_pos_current = s.z+b_height;
        int bz_neg_current = s.z;           //bottom of agent is just s.z

        //loop setup


    // Max and min integer boundraries the cubes agent collision boxes occupies

        int bx_min = s.x - box_r;
        int bx_max = s.x + box_r + 1.0;

        int by_min = s.y - box_r;
        int by_max = s.y + box_r + 1.0;

        int bz_min = s.z;
        int bz_max = s.z + b_height + 1.0;

        //state variables


        //projected positions in absence of collision detection

        // projected z position depends on whether agent is on ground 

        float cs_vx =0 ;
        float cs_vy =0 ;

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

        int bx_pos_projected = s.x+s.vx+cs_vx+box_r;    //floor
        int bx_neg_projected = s.x+s.vx+cs_vy-box_r;

        int by_pos_projected = s.y+s.vy+cs_vx+box_r;
        int by_neg_projected = s.y+s.vy+cs_vy-box_r; 
                  
        int bz_pos_projected = s.z+s.vz+cs_vx+b_height;
        int bz_neg_projected = s.z+s.vz;

    //  Float to int is floor
    // may be slow or could be optimized
    
    //cast to ints are implicit floors

    //floor, float to int
        


        int bx,by,bz;

        int xc_pos_current = 0;
        int xc_pos_projected = 0;

        int xc_neg_current = 0;
        int xc_neg_projected = 0;

        //x collisions
        for(bz = bz_min; bz<bz_max; bz++) { //less than equal
        for(by = by_min; by<by_max; by++) {
            //#x+
                if(_collision_check(bx_pos_current,by,bz))  xc_pos_current +=1;
                if(_collision_check(bx_pos_projected,by,bz)) xc_pos_projected +=1;
            //#x-
                if(_collision_check(bx_neg_current,by,bz))  xc_neg_current +=1;
                if(_collision_check(bx_neg_projected,by,bz)) xc_neg_projected +=1;
        }}

        //y collisions
        int yc_pos_current = 0;
        int yc_pos_projected = 0;

        int yc_neg_current = 0;
        int yc_neg_projected = 0;

        for(bz = bz_min; bz < bz_max; bz++){
        for(bx = bx_min; bx < bx_max; bx++){
            //#y+
                if(_collision_check(bx,by_pos_current,bz))   yc_pos_current +=1;
                if(_collision_check(bx,by_pos_projected,bz)) yc_pos_projected +=1;
            //#y-
                if(_collision_check(bx,by_neg_current,bz))   yc_neg_current +=1;
                if(_collision_check(bx,by_neg_projected,bz)) yc_neg_projected +=1;
        }}

        //z collisions

        int zc_pos_current = 0;
        int zc_pos_projected = 0;

        int zc_neg_current = 0;
        int zc_neg_projected = 0;
                
        int zc_ground_position = s.z - ground_distance; //checks if player is in contact with ground

        int on_ground = 0;

        for(bx = bx_min; bx < bx_max; bx++){
        for(by = by_min; by < by_max; by++){
            //ground contact
                if(_collision_check(bx,by, zc_ground_position)) on_ground += 1;
            //#z+
                if(_collision_check(bx,by,bz_pos_current))   zc_pos_current +=1;
                if(_collision_check(bx,by,bz_pos_projected)) zc_pos_projected +=1;
            //#z-
                if(_collision_check(bx,by,bz_neg_current))   zc_neg_current +=1;
                if(_collision_check(bx,by,bz_neg_projected)) zc_neg_projected +=1;
        }}

        //compute state variables

        //collision handling
        int total_current_collisions = xc_pos_current + xc_neg_current + yc_pos_current + yc_neg_current + zc_pos_current + zc_neg_current;
        if(total_current_collisions > 0) {
            printf("%i current collisions this frame\n", total_current_collisions);
            //push agent out of block
        }

        //handle x collisions
        if(xc_pos_current == 0 && xc_neg_current == 0)
        {
            if(xc_pos_projected != 0) s.vx = 0.0;
            if(xc_neg_projected != 0) s.vx = 0.0;
        }

        //handle y collisions

        if(yc_pos_current ==0 && yc_neg_current ==0)
        {
            if(yc_pos_projected != 0) s.vy = 0;
            if(yc_neg_projected != 0) s.vy = 0;
        }

        //handle z collision

        //#Hard collision predicted and not inside of something already
        if(zc_neg_projected != 0 && zc_neg_current == 0)
        {
            if(s.vz < 0)
            {
                if(s.vz < -z_bounce_v_threshold)
                {
                    s.vz *= -1 *z_bounce;
                }
                else
                {
                    s.vz = 0;
                }
            }
        }

        if(zc_neg_current != 0) // #if agent is inside of block, float them out the top
        {
            s.z += 0.50 / tr;
        }

        if(zc_pos_current != 0) // #if agent is inside of block, float them out the top
        {
            s.z += -0.50 / tr;
        }

        //jetpack handling
        if(!on_ground) {
            if(s.z>0)
            {
                s.vz += z_gravity;
            } 
            else 
            {
                s.vz -= z_gravity;
            }    
        }
        if(jetpack) {
            s.vz += z_jetpack;
        }
        
        //newton intergrate positions
        s.x += s.vx + cs_vx;    //use better intergrator
        s.y += s.vy + cs_vy;
        s.z += s.vz;
    
    //    printf("cs_vx= %f, cs_vy= %f \n", cs_vx, cs_vy);
    //    printf("dx= %f, dy= %f, dz= %f \n", s.vx + cs_vx, s.vy + cs_vy, s.vz);
    //   printf("cs_seq= %i, x= %f, y= %f, z= %f, vx= %f, vy= %f, vz= %f \n",cs_seq, s.x, s.y, s.z, s.vx, s.vy, s.vz);
    
        //printf("vx= %f, vy= %f, vz= %f \n", s.vx, s.vy, s.vz);
    } //end physics loop
    //printf("_tick: processed %i agent ticks\n", _tc);

}
*/

/*
## determine if agent is on ground and if they are colliding with anything at current position
        zc_current = 0
        zc_ground = 0
        zc_neg_projected = 0
        bz_current = float(z - b_height)
        bz_ground = floor(z - b_height - z_margin)
        bz_neg_projected = floor(z+vz-b_height)

        for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
                if self.collisionDetection(bx,by,bz_current):
                    zc_current +=1
                if self.collisionDetection(bx,by,bz_ground):
                     zc_ground += 1
                if self.collisionDetection(bx,by,bz_neg_projected):
                    zc_neg_projected +=1
*/
    /*
        bx_pos_current = floor(x+box_r)
        bx_pos_projected = floor(x+vx+box_r)

        bx_neg_current = floor(x-box_r)
        bx_neg_projected = floor(x+vx-box_r)

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            #x+
                if self.collisionDetection(bx,by_pos_current,bz):
                    yc_pos_current +=1
                if self.collisionDetection(bx,by_pos_projected,bz):
                    yc_pos_projected +=1
            #x-
                if self.collisionDetection(bx,by_neg_current,bz):
                    yc_neg_current +=1
                if self.collisionDetection(bx,by_neg_projected,bz):
                    yc_neg_projected +=1
    */


/*
        x,y,z, vx,vy,vz, ax,ay,az = self.state
        ax,ay,az = (0,0,0)
        vx,vy = (0,0)
        #constants
        tr = 10. #tick rate
        tr2 = tr**2 #tick rate squared
        xy_brake = math.pow(.50, 1/(float(tr))) #in percent per second
        xy_speed = 2. / tr
        if GameStateGlobal.apply_gravity:
            z_gravity = -.40 / tr2
        else:
            z_gravity = 0
        z_jetpack = 0.80 / tr2
        #gravity
    #TODO: should turn gravity off if agent is in contact with ground
        #velocity from acceleration and inputs

        vx += ax + self.v_x*xy_speed
        vy += ay + self.v_y*xy_speed

        #print str((vx,vy))
        if self.brake != 0:
            if GameStateGlobal.apply_gravity:
                vx *= xy_brake
                vy *= xy_brake
                vz *= xy_brake
            else:
                vz += -0.40 / tr2

    #constants for collision box
        b_height = self.b_height
        t_height = self.t_height
        box_r = self.box_r

### Collisions on X axis collision ###

        xc_pos_current = 0
        xc_pos_projected = 0

        xc_neg_current = 0
        xc_neg_projected = 0

        bx_pos_current = floor(x+box_r)
        bx_pos_projected = floor(x+vx+box_r)

        bx_neg_current = floor(x-box_r)
        bx_neg_projected = floor(x+vx-box_r)

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
            #x+
                if self.collisionDetection(bx_pos_current,by,bz):
                    xc_pos_current +=1
                if self.collisionDetection(bx_pos_projected,by,bz):
                    xc_pos_projected +=1
            #x-
                if self.collisionDetection(bx_neg_current,by,bz):
                    xc_neg_current +=1
                if self.collisionDetection(bx_neg_projected,by,bz):
                    xc_neg_projected +=1

### Collision on Y axis ###

        yc_pos_current = 0
        yc_pos_projected = 0

        yc_neg_current = 0
        yc_neg_projected = 0

        by_pos_current = floor(y+box_r)
        by_pos_projected = floor(y+vy+box_r)

        by_neg_current = floor(y-box_r)
        by_neg_projected = floor(y+vy-box_r)

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            #x+
                if self.collisionDetection(bx,by_pos_current,bz):
                    yc_pos_current +=1
                if self.collisionDetection(bx,by_pos_projected,bz):
                    yc_pos_projected +=1
            #x-
                if self.collisionDetection(bx,by_neg_current,bz):
                    yc_neg_current +=1
                if self.collisionDetection(bx,by_neg_projected,bz):
                    yc_neg_projected +=1

### XY Collision ###

        xyc_projected = 0

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y+vy-box_r)), int(floor(y+vy+box_r)+1)):
                for bx in range(int(floor(x+vx-box_r+vx)), int(floor(x+vx+box_r)+1)):
                    if self.collisionDetection(bx,by,bz):
                        xyc_projected += 1

        xyc_current = 0

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
                for bx in range(int(floor(x-box_r+vx)), int(floor(x+box_r)+1)):
                    if self.collisionDetection(bx,by,bz):
                        xyc_current += 1

        #dont do this right now

        if False and xyc_projected != 0:
            # print "Projected XY collision!"
            vx =0
            vy =0

### XY collision constants
        xy_bounce = .65
        xy_bounce_v_threshold = 0.35 / tr

    ## handle x collisions
        #xc_pos_current, xc_pos_projected , xc_neg_current, xc_neg_projected

        if xc_pos_current == 0 and xc_neg_current == 0:
            if xc_pos_projected != 0:
                vx = 0
            if xc_neg_projected != 0:
                vx = 0
        else:
            pass
            #print "X collision error!!"

    ## handle y collisions
        #yc_pos_current, yc_pos_projected , yc_neg_current, yc_neg_projected

        if yc_pos_current ==0 and yc_neg_current ==0:
            if yc_pos_projected != 0:
                vy = 0
            if yc_neg_projected != 0:
                vy = 0
        else:
            pass
            #print "Y collision error!!"


        #environmental and input controls
        on_ground = self.on_ground #environmental state
        jetpack = self.jetpack #control state

        if on_ground == 0: #agent on ground
            az += (z_gravity) if z>0 else (-z_gravity)
        #jetpack adjustment to gravity
        if jetpack != 0: az += z_jetpack
    ## calculate velocity from acceleration inputs
        vz += az

##parameters for collision
        z_margin = .01
        z_bounce = .65
        z_bounce_v_threshold = 0.35 / tr

## determine if agent is on ground and if they are colliding with anything at current position
        zc_current = 0
        zc_ground = 0
        zc_neg_projected = 0
        bz_current = float(z - b_height)
        bz_ground = floor(z - b_height - z_margin)
        bz_neg_projected = floor(z+vz-b_height)

        for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
                if self.collisionDetection(bx,by,bz_current):
                    zc_current +=1
                if self.collisionDetection(bx,by,bz_ground):
                     zc_ground += 1
                if self.collisionDetection(bx,by,bz_neg_projected):
                    zc_neg_projected +=1
    ##  calculate environmental state
        #agent ground state
        if zc_ground != 0:
            if self.on_ground != 1:
                self.on_ground = 1
                #print "On ground!"
        else:
            if self.on_ground == 1:
                #print "Off ground!"
                self.on_ground = 0

    ## apply velocity
        #Hard collision predicted and not inside of something already
        if zc_neg_projected != 0 and zc_current == 0:
            if vz < 0:
                if vz < -z_bounce_v_threshold: #vertical velocity bounce treshold
                    vz *= -1 *z_bounce
                else:
                    vz = 0

        if zc_current != 0: #if agent is inside of block, float them out the top
            z += .50 / tr

        if zc_neg_projected != 0:
            #print "Predicted neg Z-Collision!"
            pass
        if zc_current != 0:
            pass
            #print "Hard current neg Z-Collision!"

## Position Change ##
        z += vz
        x += vx
        y += vy
*/
