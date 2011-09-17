
#include "net_agent.hpp"

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

//move to type pack
static inline uint8_t UPACK_uint8_t(unsigned char* buffer, int*n) {
        uint8_t d = *((uint8_t*)(buffer+*n));
        *n += sizeof(uint8_t);
        return d;
}

static inline int16_t UPACK_uint16_t(unsigned char* buffer, int*n) {
        uint16_t d = *((uint16_t*)(buffer+*n));
        *n += sizeof(uint16_t);
        return d;
}

static inline int16_t UPACK_uint32_t(unsigned char* buffer, int*n) {
        int16_t d = *((int32_t*)(buffer+*n));
        *n += sizeof(int32_t);
        return d;
}

static inline float UPACK_float(unsigned char* buffer, int*n) {
        float d = *((float*)(buffer+*n));
        *n += sizeof(float);
        return d;
}

Agent_control_state_message::Agent_control_state_message() {
    id = 3;
    seq = 0;
}

void Agent_control_state_message::register_message() {
    int size = sizeof(uint8_t)*2+sizeof(uint16_t)*2+sizeof(uint32_t);
    register_message_handler(id, size, &handle_Agent_control_state_message);
}

void Agent_control_state_message::send_message() {
    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();
    int bcount = *buff_n;

    PACK_uint8_t(3, buff, buff_n);  //push message id on stack
    PACK_uint16_t(id, buff, buff_n); //agent id
    ///assume agent id is part of state?
    PACK_uint8_t(seq, buff, buff_n);
    PACK_uint16_t(tick%65536, buff, buff_n);
    PACK_uint32_t(cs, buff, buff_n);
    PACK_float(theta, buff, buff_n);
    PACK_float(phi, buff, buff_n);

    seq++;
}

int Agent_control_state_message::deserialize(unsigned char* buff, int buff_n) {
    //PACK_uint8_t(3, buff, buff_n);  //push message id on stack
    int _buff_n = buff_n;
    id = UPACK_uint16_t(buff, buff_n); //agent id
    seq = UPACK_uint8_t(buff, buff_n);
    tick =UPACK_uint16_t(buff, buff_n);
    cs = UPACK_uint32_t(buff, buff_n);
    theta = UPACK_float(buff, buff_n);
    phi = UPACK_float(buff, buff_n);

    return buff_n - _buff_n;
}


int handle_Agent_control_state_message(unsigned char* buff, int buff_n) {
    Agent_control_state_message cs;
    int r = cs.deserialize(buff, buff_n); //should pass in pointer for number of ints read

    printf("control state received: id=%i, seq=%i \n", cs.id, cs.seq);
    return r;
}

/*
class CRectangle {
    int x, y;
  public:
    void set_values (int,int);
    int area () {return (x*y);}
};

void CRectangle::set_values (int a, int b) {
  x = a;
  y = b;
}
*/
