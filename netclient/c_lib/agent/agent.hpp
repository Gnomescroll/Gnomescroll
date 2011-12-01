#pragma once

#include <c_lib/compat.h>

#include <c_lib/physics/vector.h>
#include <c_lib/ray_trace/ray_trace.h>

const int AGENT_MAX = 256;
#define AGENT_HEIGHT 1.8f
#define AGENT_HEIGHT_CROUCHED 0.9f
#define AGENT_BOX_RADIUS 0.4f

#define AGENT_START_HEALTH 100

#ifdef DC_CLIENT
#include <c_lib/compat_gl.h>
#include <agent/agent_vox.hpp>
void init_agent_vox_part(int id, int part, unsigned short vox_x, unsigned short vox_y, unsigned short vox_z, float vox_size);
void init_agent_vox_done(int id);
void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a);
void set_agent_limb_direction(int id, int part, float fx, float fy, float fz, float nx, float ny, float nz);
void set_agent_limb_anchor_point(int id, int part, float length, float ax, float ay, float az);

#include <c_lib/agent/agent_event.hpp>
#endif
#include <c_lib/agent/agent_status.hpp>


//store last network messsage

class AgentState {
    public:
        int seq;
        float theta;
        float phi;        
        float x,y,z;
        float vx,vy,vz;
        
        AgentState() { seq=-1;theta=0;phi=0;x=0;y=0;z=0;vx=0;vy=0;vz=0;}

        void forward_vector(float f[3]) {

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
};


#ifndef AGENT_CONTROL_STATE
#define AGENT_CONTROL_STATE
struct Agent_control_state {
    
    //int id;
    int seq;
    float theta;
    float phi;        
    uint32_t cs;

};
#endif

struct Agent_collision_box {
    float b_height; //standing height
    float c_height; //crouch height
    float box_r;
};

class Agent_state {
    private:
        class AgentState state_rollback;
        struct Agent_control_state cs[128];

        int snapshot_seq;
        //int rollback_seq;

        //int cs_window_min;
        //int cs_window_max;

        int cs_seq; // <--current counter

    public:    
        class AgentState s; //state current
        class AgentState state_snapshot;

        int id;
        int client_id;
        int owner;
        
        struct Agent_collision_box box;

        int _new_control_state; //Deprecate

        class Agent_status status;

        #ifdef DC_CLIENT
        class Agent_vox* vox;
        class Agent_event event;
        #endif

        #ifdef DC_SERVER
        #endif

        void _tick();
        //inline void __tick(); //does the actual work

        void set_state(float  _x, float _y, float _z, float _vx, float _vy, float _vz);
        void teleport(float x,float y,float z); //should only be used on server

        void client_tick();
        void server_tick();
        /*
            server tick:
            -advances agent position on server
            -sends state snapshot every second
        */
    
        void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz);

        //this is for client
        void handle_control_state(int _seq, int _cs, float _theta, float _phi);

        void revert_to_snapshot() {
            s = state_snapshot;
            cs_seq = state_snapshot.seq;
            //cs_window_min = state_snapshot.seq;
        }

        void revert_to_rollback() {
            s = state_rollback;            
            cs_seq = state_rollback.seq;
        }

        //deprecate
        int tick_n; //increment when ticking
        //deprecate
        int ctick;  //increment when control state received

        //int last_direction_change;
        int last_control_state_update_message;  //acts like ghost for now
        int last_full_state_message;

        void draw();

        Agent_state(int id); //default constructor
        Agent_state(int id, int owner, float x, float y, float z, float vx, float vy, float vz);

        //void server_tick();
        //set_control_state(int[8] _cs, float theta, float phi);
};

inline class AgentState _agent_tick(struct Agent_control_state _cs, const struct Agent_collision_box box, class AgentState as);

#include <c_lib/template/object_list.hpp>
class Agent_list: public Object_list<Agent_state,AGENT_MAX>
{
    private:
        const char* name() { return "Agent"; }
    public:
        void draw();

        int ids_in_use[AGENT_MAX];

        Agent_state* filtered_agents[AGENT_MAX]; // tmp array for filtering agents
        int agents_within_sphere(float x, float y, float z, float radius) {
            int ct = 0;
            float dist;
            
            int i;
            for (i=0; i<AGENT_MAX; i++) {
                if (a[i] == NULL) continue;
                dist = distance(x,y,z, a[i]->s.x, a[i]->s.y, a[i]->s.z);
                if (dist < radius) {
                    // agent in sphere
                    filtered_agents[ct] = a[i];
                    ct++;
                }
            }
            return ct;
        }

        Agent_state* hitscan_agents(float x, float y, float z, float vx, float vy, float vz, float pos[3], float* rad2, float* distance, int ignore_id) {
            int i;
            
            float _trad2=0.0f, *trad2=&_trad2;
            float dist;
            float min_dist = 100000.0f; // far away
            Agent_state* agent = NULL;
            float tpos[3];
            for (i=0; i<AGENT_MAX; i++) {
                if (a[i] == NULL) continue;
                if (a[i]->id == ignore_id) continue;
                dist = sphere_line_distance(x,y,z, vx,vy,vz, a[i]->s.x + a[i]->box.box_r, a[i]->s.y + a[i]->box.box_r, a[i]->s.z + 0.5*a[i]->box.b_height, tpos, trad2);
                if (dist < 0.0f || dist > min_dist) continue;
                if (*trad2 > 2.0f) continue;
                min_dist = dist;
                agent = a[i];
                rad2 = trad2;
                pos = tpos;
            }
            *distance = min_dist;
            return agent;
        }

        void send_to_client(int client_id);

        int get_ids(int* p) {
            p = ids_in_use;
            return get_ids();
        }

        int get_ids() {
            int i,j=0;
            for (i=0; i<AGENT_MAX;i++) {
                if (a[i] == NULL) continue;
                j++;
                ids_in_use[j] = a[i]->id;
            }
            return j;            
        }
};
