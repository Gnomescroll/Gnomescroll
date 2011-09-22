
#include "net_agent.hpp"


#include <net_lib/common/message_handler.h>
#include <c_lib/agent/agent.hpp>

/*
    agent control state message
*/

#define Agent_control_state_message_id 3
#define Agent_control_state_message_size 2*sizeof(uint8_t)+2*sizeof(uint16_t)+3*sizeof(uint32_t)

void handle_agent_control_state_message(unsigned char* buff, int buff_n, int* read_bytes) {
    Agent_control_state_message cs;
    cs.deserialize(buff, buff_n, read_bytes); //should pass in pointer for number of ints read

    //printf("Agent control state received: id=%i, seq=%i \n", cs.id, cs.seq);
}

typedef void (*pt2handler)(unsigned char*, int, int* read_bytes);
void Agent_control_state_message_register_message() {
    register_message_handler(Agent_control_state_message_id, Agent_control_state_message_size, (pt2handler) &handle_agent_control_state_message);
    printf("Registering handler for agent control state input: message_id= %i, message_size= %i\n", Agent_control_state_message_id, Agent_control_state_message_size);
}

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

Agent_control_state_message::Agent_control_state_message() {
    id = 3;
    seq = 0;
}

void Agent_control_state_message::send_message() {
    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();

    int bcount = *buff_n;
    if(*buff_n > 800) {
        printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n);
        return;
    }

    PACK_uint8_t(Agent_control_state_message_id, buff, buff_n);  //push message id on stack
    PACK_uint16_t(id, buff, buff_n); //agent id
    ///assume agent id is part of state?
    PACK_uint8_t(seq, buff, buff_n);
    PACK_uint16_t(tick%65536, buff, buff_n);
    PACK_uint32_t(cs, buff, buff_n);
    PACK_float(theta, buff, buff_n);
    PACK_float(phi, buff, buff_n);

    seq++;
    //printf("Agent_control_state_message::send_message: message size= %i bytes\n", *buff_n - bcount);
}

int Agent_control_state_message::deserialize(unsigned char* buff, int buff_n, int* read_bytes) {
    //PACK_uint8_t(3, buff, buff_n);  //push message id on stack
    int _buff_n = buff_n;
    int msg_id = UPACK_uint8_t(buff, &buff_n); //msg id, not used
    id = UPACK_uint16_t(buff, &buff_n); //agent id
    seq = UPACK_uint8_t(buff, &buff_n);
    tick =UPACK_uint16_t(buff, &buff_n);
    cs = UPACK_uint32_t(buff, &buff_n);
    theta = UPACK_float(buff, &buff_n);
    phi = UPACK_float(buff, &buff_n);

    *read_bytes = buff_n - _buff_n;
}

/*
    agent state message
*/

/*
    public:
        int id;
        int seq;
        int tick;
        
        float x,y,x;
        float vx,vy,vz;
*/

#define Agent_state_message_id 4
#define Agent_state_message_size 2*sizeof(uint8_t)+2*sizeof(uint16_t)+6*sizeof(uint32_t)

int handle_agent_state_message(unsigned char* buff, int buff_n) {
    Agent_state_message cs;
    int r = cs.deserialize(buff, buff_n); //should pass in pointer for number of ints read

    printf("Agent state received: id=%i, seq=%i \n", cs.id, cs.seq);

    //check id, if id does not exist, then create agent on receipt
    return r;
}

typedef void (*pt2handler)(unsigned char*, int, int*);
void Agent_state_message_register_message() {
    register_message_handler(Agent_state_message_id, Agent_state_message_size, (pt2handler) &handle_agent_control_state_message);
    printf("Registering handler for agent control state input: message_id= %i, message_size= %i\n", Agent_control_state_message_id, Agent_control_state_message_size);
}

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

Agent_state_message::Agent_control_state_message() {
    id = 0;
    seq = 0;
    tick = 0;

    x=0;y=0;z=0;
    vx=0;vy=0;vz=0;
}

void Agent_state_message::send_message() {
    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();

    int bcount = *buff_n;
    if(*buff_n > 800) {
        printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n);
        return;
    }

    PACK_uint8_t(Agent_state_message_id, buff, buff_n);  //push message id on stack
    PACK_uint16_t(id, buff, buff_n); //agent id
    PACK_uint8_t(seq, buff, buff_n);
    PACK_uint16_t(tick%65536, buff, buff_n);

    PACK_float(x, buff, buff_n);
    PACK_float(y, buff, buff_n);
    PACK_float(z, buff, buff_n);
    PACK_float(vx, buff, buff_n);
    PACK_float(vy, buff, buff_n);
    PACK_float(vz, buff, buff_n);

    seq++;
    //printf("Agent_control_state_message::send_message: message size= %i bytes\n", *buff_n - bcount);
}

void Agent_state_message::deserialize(unsigned char* buff, int buff_n, int* read_bytes) {
    //PACK_uint8_t(3, buff, buff_n);  //push message id on stack
    int _buff_n = buff_n;
    int msg_id = UPACK_uint8_t(buff, &buff_n); //msg id, not used
    id = UPACK_uint16_t(buff, &buff_n); //agent id
    seq = UPACK_uint8_t(buff, &buff_n);
    tick =UPACK_uint16_t(buff, &buff_n)

    x = UPACK_float(buff, &buff_n);
    y = UPACK_float(buff, &buff_n);
    z = UPACK_float(buff, &buff_n);
    vx = UPACK_float(buff, &buff_n);
    vy = UPACK_float(buff, &buff_n);
    vz = UPACK_float(buff, &buff_n);

    *read_bytes = buff_n - _buff_n;
}
