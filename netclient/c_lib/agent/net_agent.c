
static inline void pack_NET_agent_snapshot(unsigned char* buffer, struct NET_agent_snapshot* s, int* n) {
    PACK_int16_t(s->id, buffer, n);
    PACK_int16_t(s->tick, buffer, n);
    PACK_float(s->x, buffer, n);
    PACK_float(s->y, buffer, n);
    PACK_float(s->z, buffer, n);
    PACK_float(s->vx, buffer, n);
    PACK_float(s->vy, buffer, n);
    PACK_float(s->vz, buffer, n);
    //*n += sizeof(int16_t)+sizeof(int16_t)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+0;;
}

static inline void unpack_NET_agent_snapshot(unsigned char* buffer, struct NET_agent_snapshot* s, int* n) {
    UNPACK_float(&s->vz, buffer, n);
    UNPACK_float(&s->vy, buffer, n);
    UNPACK_float(&s->vx, buffer, n);
    UNPACK_float(&s->z, buffer, n);
    UNPACK_float(&s->y, buffer, n);
    UNPACK_float(&s->x, buffer, n);
    UNPACK_int16_t(&s->tick, buffer, n);
    UNPACK_int16_t(&s->id, buffer, n);
    //*n += sizeof(int16_t)+sizeof(int16_t)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+0;;
}

static inline void pack_NET_agent_control_state(unsigned char* buffer, struct NET_agent_control_state* s, int* n) {
    PACK_int16_t(s->id, buffer, n);
    PACK_int16_t(s->tick, buffer, n);
    PACK_uint8_t(s->seq, buffer, n);
    PACK_uint8_t(s->theta, buffer, n);
    PACK_uint8_t(s->phi, buffer, n);
    PACK_uint32_t(s->cs, buffer, n);
    //*n += sizeof(int16_t)+sizeof(int16_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint32_t)+0;;
}

static inline void unpack_NET_agent_control_state(unsigned char* buffer, struct NET_agent_control_state* s, int* n) {
    UNPACK_uint32_t(&s->cs, buffer, n);
    UNPACK_uint8_t(&s->phi, buffer, n);
    UNPACK_uint8_t(&s->theta, buffer, n);
    UNPACK_uint8_t(&s->seq, buffer, n);
    UNPACK_int16_t(&s->tick, buffer, n);
    UNPACK_int16_t(&s->id, buffer, n);
    //*n += sizeof(int16_t)+sizeof(int16_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint32_t)+0;;
}


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

void client_init_net_agent() {

}

void server_init_net_agent() {

}

/*
     int i;
    uint32_t _n = 1;
    for(i=0;i<6;i++) {
        if((flag & _n) != 0) {
            s[i] = 1;
        } else {
            s[i] = 0;
        }
        _n *= 2;
    }
*/

/*
     int i;
    uint32_t _n = 1;
    uint32_t flag = 0;
    for(i=0;i<6;i++) {
        if(CS.s[i] != 0) {
            flag |= _n;
        }
        _n *= 2;
    }
*/

void handle_NET_agent_control_state(unsigned char* buff, int n) {
    struct NET_agent_control_state ncs;
    unpack_NET_agent_control_state(buff, &ncs, &n);

    struct Agent_cs cs;
    int id = cs.id;
    cs.tick = ncs.tick;
    cs.seq = ncs.seq;
    cs.theta = ncs.theta;
    cs.phi = ncs.phi;
    cs.cs = ncs.cs;

    apply_agent_snapshot(id, &as);
}
/*
struct Agent_cs { //control state
    int tick;
    int seq;
    float theta,phi;
    int cs[CSn]; //movement, jetpack
};
*/
void handle_NET_agent_snapshot(unsigned char* buff, int n) {
    struct NET_agent_snapshot ncs;
    unpack_NET_agent_snapshot(buff, &ncs, &n);

    struct Agent_snapshot cs;

/*
struct Agent_snapshot {
    int id; //ignore on client
    int tick;
    float x,y,z;
    float vx,vy,vz;
    //float theta,phi;
}
*/
}
