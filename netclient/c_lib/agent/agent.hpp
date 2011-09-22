#pragma once

	
class Agent_list {

    public:
        int num
        Agent_state* a[1024];

        Agent_list(); //default constructor
        Agent_state* get(int id);
        Agent_state* new_agent();
        void delete_agent();

        void draw();
        void tick();
};


Agent_list::Agent_list(){
	num = 0;
	int i;
	for(i=0;i<1024;i++) a[i] = NULL;
}

Agent_state* Agent_list::get(int id) {
	if((i < 0) || (i >= 1024)) {
		printf("Agent id error: id=%i\n",id);
		return NULL;
	} 
	if(a[i] == NULL) {
		printf("agent get error: agent is null, id=%i\n", id);
		return NULL;
	}
}

Agent_state* Agent_list::new_agent(int i) {
	


}

void Agent_list::delete_agent() {
	
	printf("Agent_list::delete_agent not implemented\n");
}

void Agent_list::draw() {
	
	printf("Agent_list::draw, not implemented\n");
}

void Agent_list::tick() {
	
	printf("Agent_list::tick, not implemented\n");
}