

struct client_control_state {
    int seq;
    uint32_t cs;
    unsigned int tick;
    float theta;
    float phi;
}

struct client_agent_state {
    int id;

    int seq;
    struct client_control_state cs[128];

}

struct client_agent_state active_agent;

void init_agent_client() {
    active_agent.id = 0;
    init_client_agent_start(&active_agent);
}

void init_client_agent_start(struct client_agent_state* a) {
    a->seq = 0;
    int i;
    for(i=0;i<129;i++) {
        a->cs[i].cs = 0;
        a->cs[i].seq = -1;
        a->cs[i].tick = -1;
    }
}

void set_control_state(int x[32], float theta, float phi) {
    struct client_agent_state* a = &active_agent;

    a->seq = (a->seq+1)%128;
    cs[a->seq].seq = a->seq;
    int ctick = _get_tick();
    cs[a->seq].seq.tick = ctick;

    uint32_t _n = 1;
    uint32_t flag = 0;
    for(i=0;i<32;i++) {
        if(CS.s[i] != 0) {
            flag |= _n;
        }
        _n *= 2;
    }

    cs[a->seq].seq.cs= flag;
    cs[a->seq].theta = theta; //should be a byte
    cs[a->seq].phi = phi; //should be a byte

    char* buff= get_client_out_buffer();
    int* buff_n = get_client_out_buffer_n();

    PACK_uint8_t(3, buff, buff_n);  //push message id on stack
    PACK_uint16_t(id, buff, buff_n); //agent id
    ///assume agent id is part of state?
    PACK_uint8_t(a->seq, buff, buff_n);
    PACK_uint16_t(ctick%65536, buff, buff_n);
    PACK_uint32_t(flag, buff, buff_n);
    PACK_float(theta, buff, buff_n);
    PACK_float(phi, buff, buff_n);
}

