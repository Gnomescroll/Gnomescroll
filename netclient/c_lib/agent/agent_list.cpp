#include "agent_list.hpp"

#ifdef DC_CLIENT
        #include <c_lib/agent/agent_draw.hpp>
#endif

Agent_list::Agent_list(){
	num = 0;
	id_c = 0;
	int i;
	for(i=0;i<1024;i++) a[i] = NULL;
}

Agent_state* Agent_list::get(int id) {
	int i;
	if((i < 0) || (i >= 1024)) {
		printf("Agent id error: id=%i\n",id);
		return NULL;
	} 
	if(a[i] == NULL) {
		printf("agent get error: agent is null, id=%i\n", id);
		return NULL;
	}
	return a[i];
}

Agent_state* Agent_list::new_agent() {
	int i;
	int id = id_c;
	id_c++;
	for(i=0; i<1024;i++) {
		id = (i+id)%1024;
		if(a[id] == NULL) break;
	}
	if(i==1024) {
		printf("Error: cannot create agent, agent limit exceeded\n");
		return NULL;
	}
	num++;
	a[id] = new Agent_state(id);
	printf("Created agent %i\n", id);
}

void Agent_list::delete_agent(int id) {
	if(a[i]==NULL) {
		printf("Cannot delete agent: agent is null\n");
	}
	delete a[i];
	a[i] == NULL;
	num--;
	printf("Deleted agent %i\n", id);
	//printf("Agent_list::delete_agent not implemented\n");
}

void Agent_list::draw() {
	
	printf("Agent_list::draw, not implemented\n");

	#ifdef DC_CLIENT
	Agent_draw::draw_agents(&this);
	#endif
}

void Agent_list::tick() {
	
	printf("Agent_list::tick, not implemented\n");
}