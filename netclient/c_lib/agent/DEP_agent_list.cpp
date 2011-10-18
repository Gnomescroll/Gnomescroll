#include "agent_list.hpp"

#ifdef DC_CLIENT
        #include <c_lib/agent/agent_draw.hpp>
#endif

const int AGENT_LIST_MAX 1024;

Agent_list::Agent_list(){
    num = 0;
    id_c = 0;
    int i;
    for(i=0;i<AGENT_LIST_MAX;i++) a[i] = NULL;
}

Agent_state* Agent_list::get(int id) {
    if((id < 0) || (id >= AGENT_LIST_MAX)) {
        printf("Agent id error: id=%i\n",id);
        return NULL;
    } 
    if(a[id] == NULL) {
        printf("agent get error: agent is null, id=%i\n", id);
        return NULL;
    }
    return a[id];
}

Agent_state* Agent_list::new_agent() {
    int i;
    int id = id_c;
    id_c++;
    for(i=0; i<AGENT_LIST_MAX;i++) {
        id = (i+id)%AGENT_LIST_MAX;
        if(a[id] == NULL) break;
    }
    if(i==AGENT_LIST_MAX) {
        printf("Error: cannot create agent, agent limit exceeded\n");
        return NULL;
    }
    num++;
    a[id] = new Agent_state(id);
    printf("Created agent %i\n", id);
}

void Agent_list::delete_agent(int id) {
    if(a[id]==NULL) {
        printf("Cannot delete agent: agent is null\n");
        return;
    }
    delete a[id];
    a[id] = NULL;
    num--;
    printf("Deleted agent %i\n", id);
    //printf("Agent_list::delete_agent not implemented\n");
}

void Agent_list::draw() {
    
    printf("Agent_list::draw, not implemented\n");

    #ifdef DC_CLIENT
    AgentDraw::draw_agents(this);
    #endif
}

void Agent_list::tick() {
    
    printf("Agent_list::tick, not implemented\n");
}
