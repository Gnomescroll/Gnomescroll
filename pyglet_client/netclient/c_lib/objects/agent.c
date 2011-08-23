#include "agent.h"

#DEFINE NAGENTS 1024
uint agent_id_counter=1;
struct Agent_state* Agent_list[1024];

int a_count = 0;

struct Agent_state* get_agent(uint id) {
    struct Agent_state* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i]->id == id) {
            return Agent_list[i]
        }
    }
    return NULL;
}

void agent_tick() {

}

void agent_draw() {

}


int create_agent(float x, float y, float z) {

    struct Agent_state* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i] == NULL) {
            g = (struct Agent_state *) malloc (sizeof(struct Agent_state));
            Agent_list[i] = g;
            a_count++;
            break;
        }
    }
    if(a== NULL) { printf("Bug: max grenade number reached!\n"); return;}
    g->id = agent_id_counter;
    printf("Agent Created: id= %i \n", g->id);
    agent_id_counter++;
    g->x=x;
    g->y=y;
    g->z=z;
    g->vx=0;
    g->vy=0;
    g->vz=0;
    g->xangle=0;
    g->yangle=0;
    g->camera_height= 2.5;
    g->cbox_height= 3.0;
    g->cbox_radius = 0.45;
}

void set_agent_state(int id, float xangle, float yangle) {
    Agent_state* g = get_agent(id)
    if(g==NULL) { printf("agent.set_agent_state: agent id does not exist: %i \n", id); return;}

    g->xangle = xangle;
    y->yangle = yangle;
}
