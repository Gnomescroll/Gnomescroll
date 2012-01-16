

struct client_agent_control_state {
    int seq;
    uint32_t cs;
    unsigned int tick;
    float theta;
    float phi;
};

struct client_agent_state {
    int id;

    int seq;
    struct client_control_state cs[128];

};


//

struct server_agent_control_state {
    int seq;
    uint32_t cs;
    unsigned int tick;
    float theta;
    float phi;
};

struct server_agent_state {
    int id;

    int ctime;
    int highest_seq;

    //int seq;
    struct server_control_state cs[128];

    float x,y,z;
    float vx,vy,vz;
};

//


int handle_agent_control_state_message(unsigned char* buff, int n) {

    uint16_t id;
    uint8_t seq;
    uint16_t ctick;
    uint32_t flag;
    float theta;
    float phi;

    int n1 = n;
    //UNPACK_uint8_t(3, buff, n1);  //push message id on stack
    UNPACK_uint16_t(&id, buff, n1); //agent id
    ///assume agent id is part of state?
    UNPACK_uint8_t(&seq, buff, n1);
    UNPACK_uint16_t(&ctick, buff, n1);
    UNPACK_uint32_t(&flag, buff, n1);
    UNPACK_float(&theta, buff, n1);
    UNPACK_float(&phi, buff, n1);

    printf("agent_control_state_message is %i bytes\n", n1-n);
    //get agent
    struct server_agent_state* a = get_agent(id);
    if(a == NULL) {
        printf("WTF\n");
        return n1-n;
    }
    if(a->seq < seq) {
        //discard packet
    } else {
        //process packet
        a->seq = seq;
    }
    return n1-n;
}


struct server_agent_state _ta;

struct server_agent_state* get_agent(int id) {
    return &_ta;
}

void create_agent() {
    _ta.id = 0;
    //_ta.seq = 0;
    _ta.ctime = -1;

    _ta.x = 5;
    _ta.y = 5;
    _ta.z = 10;
    _ta.vx = 0;
    _ta.vy = 0;
    _ta.vz = 0;

}

void init_agent_server() {
    printf("Server Message Registered: handle_agent_control_state_message\n");
    register_message_handler(1, 18, &handle_agent_control_state_message); //id, size, function
}
