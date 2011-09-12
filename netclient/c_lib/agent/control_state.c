#include "control_state.h"

void init_agent_control_state() {

    //printf("TEST\n");

    #ifdef DC_CLIENT

    #endif


    #ifdef DC_SERVER

    printf("Message Registered: handle_agent_control_state_message\n");
    register_message_handler(1, 18, &handle_agent_control_state_message); //id, size, function

    #endif

}


#ifdef DC_CLIENT

struct Control_state {
    uint16_t id;
    uint16_t seq;
    uint16_t tick;
    float xangle, yangle;
    int s[6];
};

struct Control_state CS;
int CS_SEQ = 0;

void set_agent_control_state(int* cs, float xangle, float yangle) {
    int i;
    for(i=0;i<6; i++) CS.s[i] = cs[i];
    CS.xangle = xangle;
    CS.yangle = yangle;
    CS.seq = CS_SEQ;
    CS_SEQ++;
}

void PACK_control_state(unsigned char *buff, int* n) {
    PACK_uint8_t(1, buff, n);  //push message id on stack
    PACK_uint16_t(CS.id, buff, n);
    PACK_uint16_t(CS.seq, buff, n);
    PACK_uint16_t(CS.tick, buff, n);
    PACK_float(CS.xangle, buff, n);
    PACK_float(CS.yangle, buff, n);

    int i;
    uint32_t _n = 1;
    uint32_t flag = 0;
    for(i=0;i<6;i++) {
        if(CS.s[i] != 0) {
            flag |= _n;
        }
        _n *= 2;
    }
    PACK_uint32_t(flag, buff, n);

    //PACK_uint32_t(uint32_t d, unsigned char* buffer, int* n)

}
#endif


#ifdef DC_SERVER

int handle_agent_control_state_message(unsigned char* buff, int n) {
    int in = n;
    uint16_t id;
    uint16_t seq;
    uint16_t tick;
    float xangle;
    float yangle;
    uint32_t flag;
    int s[6];

    UNPACK_uint16_t(&id, buff, &n);
    UNPACK_uint16_t(&seq, buff, &n);
    UNPACK_uint16_t(&tick, buff, &n);
    UNPACK_float(&xangle, buff, &n);
    UNPACK_float(&yangle, buff, &n);
    UNPACK_uint32_t(&flag, buff, &n);

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
    //printf("id=%i seq=%i tick=%i \n", id,seq,tick);
    printf("cs= ");
    for(i=0; i<6;i++) {
        if(s[i] == 0) {
            printf("1 ");
        } else {
            printf("0 ");
        }
    }
    printf("\n");

    return n -in;
}



#endif
