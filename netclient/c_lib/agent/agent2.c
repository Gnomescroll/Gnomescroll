

struct Agent_cs { //control state
    int tick;
    int seq;
    float x,y,z;
    float theta,phi;
    int cs[6]; //movement, jetpack
};

struct Agent_snapshot {
    //int id; //ignore on client
    int tick;
    float theta,phi;

    struct Agent_ick
}

struct Agent {
    int id;
    int local;

    float x,y,z;
    float theta,phi;

    int tick; //increment every control state change

    int tbn;
    struct Agent_cs t_buffer[128];

    int sbn;
    struct Agent_snapshot s_buffer[8];

};

void create_agent(int id, struct Agent_snapshot* as) {

    struct Agent* a = malloc(sizeof(struct Agent));
    a.id = id;
    int tbn=0;
    int sbn=0;

    int i;
    for(i=0;i<8;i++) {
        a->s_buffer[i].
    }

}
