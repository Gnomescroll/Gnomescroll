
#include "agent.hpp"

#include <c_lib/agent/net_agent.cpp>

//#ifdef DC_CLIENT
//#endif

//template <>

#include <c_lib/agent/agent_draw.hpp>


void Agent_list::draw() {

#ifdef DC_CLIENT
    int i;
    struct Agent_state* g = NULL;

    //printf("Drawing agents\n");
    glDisable(GL_TEXTURE_2D);
    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    for(i=0; i<n_max; i++) { //max_n
        if(a[i] != NULL) {
            g = a[i];
            AgentDraw::draw_agent(g);
        }
    }
    glDisable(GL_CULL_FACE);
    //glEnable(GL_TEXTURE_2D);
    //glDisable(GL_DEPTH_TEST);
#endif
}


Agent_state::Agent_state(int _id) {
    id = _id;
    x = 0;
    y = 0;
    z = 0;
    vx = 0;
    vy = 0;
    vz = 0;

    s.x = 15;
    s.y = 15;
    
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
}

/*
Agent_state::handle_control_state(int seq, int cs, float theta, float phi) {
    

}
*/
/*
#ifdef DC_CLIENT
    #include <c_lib/agent/agent_draw.hpp>
#endif

Agent_state::draw() {
    printf("draw not implemented\n");
    #ifdef DC_CLIENT

    return;
    #endif
}
*/

/*
void Agent_state::client_tick() {
    return;
}
*/

void Agent_state::server_tick() {
    //printf("tick not implemented\n");
    
    tick_n++;

    //printf("agent_id=%i, new controlstate = %i\n", id, new_control_state);
    printf("Agent %i: new_control_state=  %i \n", id, _new_control_state);

    //printf("cs_seq= %i \n", cs_seq);
    //cs_seq++;

    if(_new_control_state == 1) {
        _tick();    //advance agent
    }


    if(tick_n % 30 ==0) {
        Agent_state_message A;
    /*
        int id;
        int seq;
        int tick;

        float x;
        float y;
        float z;
        float vx,vy,vz;
    */
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
        for(i=0;i<128;i++){
            if(cs[i].seq < cs_seq || cs[i].seq > cs_seq+60) cs[i].seq = -1; //clear any control state not from next 60 ticks
        }
    }

    /*
        int id;
        int seq;
        uint32_t cs;
        int tick;
        int ctick;
        float theta;
        float phi;
    */

    /*
    if(tick_n % 3 ==0) {
        Agent_control_state_to_client_message B;

        B.id = id;
        B.theta = theta;
        B.phi = phi;
        B.broadcast();
    }   
    */


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
    

void agents_tick() {
    
}
