
#include "agent.h"

/*
struct Agent_state* Agent_list[1024];
int agent_id_counter = 0;
int a_count = 0;
*/

/*
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
*/

void agent_Tick(struct Agent_state* g) {
    g->theta += 0.01;
    g->theta = 0.0;
    g->phi += 0; //0.005; //0.35;
}

// end misc

//only use on client
#ifdef DC_CLIENT
struct Agent_state* create_agent_from_snapshot(int id, struct Agent_snapshot* as) {
    struct Agent_state* a = (struct Agent_state*) malloc(sizeof(struct Agent_state));
    a->id = id;
    a->tbn=0;

    int i;

    for(i=0;i<128;i++) {
        a->t_buffer[i].tick = -1;
        a->t_buffer[i].seq = -1;
    }

    a->x = as->x;
    a->y = as->y;
    a->z = as->z;
    a->vx = as->vx;
    a->vy = as->vy;
    a->vz = as->vz;
    a->theta = 0;
    a->phi = 0;
    a->camera_height= 2.5;
    a->cbox_height= 3.0;
    a->cbox_radius = 0.45;

    a->last_snapshot = *as;

    a_count++;
/*
    for(i=0; i<6;i++) {
        g->vox_part[i].vox = NULL;
    }
*/

    return a;
}
#endif
//only use on server

#ifdef DC_SERVER
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
    if(a== NULL) { printf("Bug: max agent number reached!\n"); return -1;}
    agent_id_counter +=1;
    g->id = agent_id_counter;
    printf("Agent Created: id= %i \n", g->id);
    g->x=x;
    g->y=y;
    g->z=z;
    g->vx=0;
    g->vy=0;
    g->vz=0;
    g->theta=0;
    g->phi=0;
    g->camera_height= 2.5;
    g->cbox_height= 3.0;
    g->cbox_radius = 0.45;

    a_count++;
    return g->id;
}
#endif

void apply_control_state(int id, struct Agent_cs* cs ) {
    //get agent
    struct Agent_state* a = get_agent(id);
    if(a==NULL) {
        printf("apply_control_state: agent id does not exist: %i\n", a->id);
        }

    printf("Agent Control State Change: agent_id=%i, tick=%i\n", a->id, cs->tick);
    a->t_buffer[cs->seq % 128] = *cs;
    if(cs->tick > a->ltick) {
        a->ltick = cs->tick;
        a->tbn = cs->seq % 128;
    }
}

void apply_agent_snapshot(int id, struct Agent_snapshot* as) {
    //get agent
    struct Agent_state* a = get_agent(id);
    if(a==NULL) {
        printf("Agent id does not exist, creating: %i\n", a->id);
        }
    if(a->last_snapshot.tick < as->tick) {
        a->last_snapshot = *as;
        printf("Agent Snapshot: agent_id=%i, tick=%i\n", a->id, as->tick);
    } else {
        printf("Old snapshot received: tick=%i, newest snapshot is tick=%i\n",as->tick ,a->last_snapshot.tick);
    }
}

void reset_agent_to_last_snapshot(struct Agent_state* a) {
    /*
    struct Agent_state* a = get_agent(id);
    if(a==NULL) {
        printf("Agent id does not exist: %i\n", a->id);
        }
    */
    struct Agent_snapshot* as = &a->last_snapshot;

    a->x = as->x;
    a->y = as->y;
    a->z = as->z;
    a->vx = as->vx;
    a->vy = as->vy;
    a->vz = as->vz;
    //get theta/phi from control state
    //a->theta = as->theta;
    //a->phi = as->phi;
}

void update_agent_position(int id) {
    /*
        Algorithm:
        -reset agent state to last snapshot
        -find control states starting at last snapshot
        -replay agent physics tick by tick until we run out of control states since last tick
        -use dead reckoning to go from last update to current tick
    */
    /*
    struct Agent_state* a = get_agent(id);
    if(a==NULL) {
        printf("Agent id does not exist: %i\n", a->id);
        }

    int tick = a->last_snapshot.tick;

    int i,j;
    j = a->tbn;

    int seq = -1;
    reset_agent_to_last_snapshot(a);
    for(i=0; i<128; i++) {
        if(a->t_buffer[i].tick == tick) {
            seq = i;
            break;
        }
    }
    if(seq=-1) {
        printf("update_agent_position: seq=-1, no valid control state for agent snapshot\n");
        return
    }
    i = seq %128;
    int tick0, tick1;
    while(1) {
        if(a->t_buffer[i+1].tick < tick) {
            break;
        }
        tick0 = t_buffer[i].tick;
        tick1 = t_buffer[i+1].tick;
        if(tick1 - tick0 < 0) {
            printf("tick1-tick0 is negative! %i, %i, %i\n", tick0, tick1, tick1-tick0);
            return;
        }

        i = (i+1)%128;
    }
        //do tick until next delta
    }
    */
}
