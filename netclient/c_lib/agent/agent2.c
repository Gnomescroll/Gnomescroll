
#define CSn 8 //number of control state button variables

struct Agent_cs { //control state
    int tick;
    int seq;
    float theta,phi;
    int cs[CSn]; //movement, jetpack
};

struct Agent_snapshot {
    //int id; //ignore on client
    int tick;
    float x,y,z;
    float vx,vy,vz;
    float theta,phi;
}

struct Agent {
    int id;
    int local;

    float x,y,z;
    float theta,phi;

    int tick; //increment every control state change

    int tbn;
    struct Agent_cs t_buffer[128];

    int sbn; //index to last snap shot
    struct Agent_snapshot s_buffer[8];

};

struct Agent* create_agent(int id, struct Agent_snapshot* as) {

    struct Agent* a = malloc(sizeof(struct Agent));
    a->id = id;
    a->tbn=0;
    a->sbn=0;

    int i;
    for(i=0;i<8;i++) {
        a->s_buffer[i].tick = -1;
    }
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
    a->theta = as->theta;
    a->phi = as->phi;

    return a;
}

void apply_control_state(int id, struct Agent_cs* cs ) {
    //get agent

    printf("Agent Control State Change: agent_id=%i, tick=%i\n", a->id, cs->tick);
    a->t_buffer[cs->tick % 128] = *cs;
    if(cs->tick > a->tick) {
        a->tick = cs->tick;
        cs->tbn = cs->tick % 128;
    }
}

void apply_agent_snapshot(int id, struct Agent_snapshot* as) {
    //get agent
    if(a->s_buffer[a->sbn].tick < as->tick]) {
        a->sbn = (a->sbn+1)%8;
        s_buffer[a->sbn] = *as;

        int i;
        for(i=0; i<128; i++) {

            if(a->t_buffer[i].tick < s_buffer[a->sbn]) {
                a->t_buffer[i] = -1; //wipe control state before current snapshot
            }
        }
        printf("Agent Snapshot: agent_id=%i, tick=%i\n", a-id, as->tick);
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
    int tick = a->s_buffer[a->sbn].tick;

    int i,j;
    j = a->tbn;

    reset_agent_to_last_snapshot(a);
    for(i=0; i<128; i++) {


        if(t_buffer[i].tick < )

    }


}
