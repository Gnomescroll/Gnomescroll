#include "agent.h"


struct Agent_state* Agent_list[1024];

int agent_id_counter = 0;
int a_count = 0;

void agent_Tick(struct Agent_state* g);
void agent_Draw(struct Agent_state* g);

struct Agent_state* get_agent(uint id) {
    struct Agent_state* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i]->id == id) {
            return Agent_list[i];
        }
    }
    return NULL;
}

void agent_Tick(struct Agent_state* g) {

}

void agent_tick() {

}

void agent_Draw(struct Agent_state* g) {

}

void agent_draw() {
    struct Agent_state* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i] == NULL) {
            g = Agent_list[i];
            agent_Draw(g);
        }
    }
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
    if(a== NULL) { printf("Bug: max agent number reached!\n"); return;}
    agent_id_counter +=1;
    g->id = agent_id_counter;
    printf("Agent Created: id= %i \n", g->id);
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

void destroy_agent(int id) {
    struct Agent_state* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i]->id == id) {
            g = Agent_list[i];
            break;
        }
    }
    if( g != NULL) {
        Agent_list[i] = NULL;
        free(g);
        g_count--;
    } else {
        printf("Destroy agent failed: agent does not exist, id=%i \n", id);
    }

}

void set_agent_state(int id, float xangle, float yangle) {
    struct Agent_state* g = get_agent(id);
    if(g==NULL) { printf("agent.set_agent_state: agent id does not exist: %i \n", id); return;}

    g->xangle = xangle;
    g->yangle = yangle;
    /*
    c->xl = c->x + cos( c->x_angle * PI) * cos( c->y_angle * PI);
    c->yl = c->y + sin( c->x_angle * PI) * cos( c->y_angle * PI);
    c->zl = c->z + sin( c->y_angle);

    length = sqrt(c->xl*c->xl + c->yl*c->yl + c->zl*c->zl);
    */
}

//
struct Agent_state* get_agent(int id) {
    struct Agent_state* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i]->id == id) {
            g = Agent_list[i];
            return g;
        }
    return NULL;
}

struct Vox* get_agent_vox_part(int id, int part) {
    struct Agent_state* g = get_agent(id);
    if(g==NULL) { printf("get_agent_vox_part: error \n"); return NULL; }
    struct Vox* v = &g->vox_part[part];
    return v;
}
