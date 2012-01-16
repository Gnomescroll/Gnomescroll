#include "agent_list.h"

struct Agent_state* Agent_list[1024];

int agent_id_counter = 0;
int a_count = 0;

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
        ///for(i=0; i < AGENT_PART_NUM; i++) {
        ///    destroy_vox(&g->vox_part[i]);
        ///    }


        free(g);
        a_count--;
    } else {
        printf("Destroy agent failed: agent does not exist, id=%i \n", id);
    }

}

struct Agent_state* get_agent(int id) {
    //struct Agent_state* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i] != NULL && Agent_list[i]->id == id) {
            return Agent_list[i];
        }
    }
    return NULL;
}

void agent_tick() {
    struct Agent_state* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i] != NULL) {
            g = Agent_list[i];
            agent_Tick(g);
        }
    }
}
