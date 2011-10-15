
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
}

void Agent_state::client_tick() {
        //_tick();
		//try tick on input received    
	}

void Agent_state::server_tick() {

    if(_new_control_state == 1) {
		//_tick();	//advance agent
		//tick on input received
    }

	tick_n++;
	//if(tick_n % 30 == 0) {
	if(cs_seq % 32 == 0) {
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
            if(cs[i].seq < cs_seq || cs[i].seq > cs_seq+60) {
	        	if(cs[i].seq == cs_seq) printf("!!! Error 1 \n");
	        	if(cs[i].seq == (cs_seq+1)%128) printf("!!! Error 2 \n");
	        	if(cs[i].seq == (cs_seq+25)%128) printf("!!! Error 3 \n");
	        	if(cs[i].seq == (cs_seq+55)%128) printf("!!! Error 4 \n");
	            cs[i].seq = -1; //clear any control state not from next 60 ticks
    		}    
		}
	}

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
