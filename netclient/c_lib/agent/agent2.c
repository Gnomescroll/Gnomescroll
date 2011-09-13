
#define CSn 8 //number of control state button variables



/*
struct NET_agent_snapshot {
    int16_t id;
    int16_t tick;
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
};

struct NET_agent_control_state {
    int16_t id;
    int16_t tick;
    uint8_t seq;
    uint8_t theta;
    uint8_t phi;
    uint32_t cs;
};
*/

struct Agent {
    int id;
    int local;

    float x,y,z;
    float theta,phi;

    int tick; //increment every control state change

    int tbn;
    struct Agent_cs t_buffer[128];

    struct Agent_snapshot last_snapshot;

};

void handle_agent_snapshot(char* buff, int* n) {




}


struct Agent* create_agent(int id, struct Agent_snapshot* as) {

    struct Agent* a = malloc(sizeof(struct Agent));
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
    //a->theta = as->theta;
    //a->phi = as->phi;

    a->last_snapshot = *as;
    return a;
}

void apply_control_state(int id, struct Agent_cs* cs ) {
    //get agent

    printf("Agent Control State Change: agent_id=%i, tick=%i\n", a->id, cs->tick);
    a->t_buffer[cs->seq % 128] = *cs;
    if(cs->tick > a->ltick) {
        a->tick = cs->ltick;
        cs->tbn = cs->seq % 128;
    }
}

void apply_agent_snapshot(int id, struct Agent_snapshot* as) {
    //get agent
    if(a->last_snapshot.tick < as->tick]) {
        a->last_snapshot = *as;
        printf("Agent Snapshot: agent_id=%i, tick=%i\n", a->id, as->tick);
    } else {
        printf("Old snapshot received: tick=%i, newest snapshot is tick=%i\n",as->tick ,a->s_buffer[a->sbn].tick);
    }
}

void reset_agent_to_last_snapshot(struct Agent *a) {
    struct Agent_snapshot* s = &a->s_buffer[a->sbn];

    a->x = as->x;
    a->y = as->y;
    a->z = as->z;
    a->vx = as->vx;
    a->vy = as->vy;
    a->vz = as->vz;
    a->theta = as->theta;
    a->phi = as->phi;

}

void update_agent_position(int id) {
    int tick = a->last_snapshot.tick;

    int i,j;
    j = a->tbn;

    reset_agent_to_last_snapshot(a);
    for(i=0; i<128; i++) {


        if(t_buffer[i].tick < tick) {

        }

    }


}
