
#include "agent.hpp"

#include <c_lib/agent/net_agent.cpp>

//#ifdef DC_CLIENT
//#endif

//template <>

#include <c_lib/agent/agent_draw.hpp>



void Agent_list::draw() 
{
    #ifdef DC_CLIENT
        int i;

        //printf("Drawing agents\n");
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

void Agent_state::handle_control_state(int _seq, int _cs, float _theta, float _phi) {
    //printf("control state received: seq=%i, cs=%i \n", _seq, _cs);
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
            A.vx = s.vz;
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
    x = 0;
    y = 0;
    z = 0;
    vx = 0;
    vy = 0;
    vz = 0;

    s.x = 16.5;
    s.y = 16.5;

    cs_seq = 0;

    printf("Agent_state::Agent_state, new agent\n");

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
