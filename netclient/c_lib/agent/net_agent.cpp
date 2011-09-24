#pragma once

//#include "net_agent.hpp"

#include <net_lib/client/client.hpp>
#include <net_lib/common/message_handler.h>
#include <c_lib/agent/agent.hpp>
#include <net_lib/server/server.h>

//0 for unpack, 1 for pack

static inline void pack_message_id(int message_id, unsigned char* buff, int*buff_n, bool pack) {
    if(pack == true) *((uint8_t*)(buff+*buff_n)) = (uint8_t)message_id;
    //if(pack == false); //do nothing
    *buff_n += sizeof(uint8_t);
}

static inline void pack_float(float* x, unsigned char* buff, int*buff_n, bool pack)
{
    if(pack == true) *((float*)(buff+*buff_n)) = *x;
    if(pack == false) *x = *((float*)(buff+*buff_n));
    *buff_n += sizeof(float);
}

static inline void pack_32(int* x, unsigned char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((int32_t*)(buff+*buff_n)) = *x;
    if(pack == false)   *x = *((int32_t*)(buff+*buff_n));
    *buff_n += sizeof(int32_t);
}

static inline void pack_u32(int* x, unsigned char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((uint8_t*)(buff+*buff_n)) = *x;
    if(pack == false)   *x = *((uint32_t*)(buff+*buff_n));
    *buff_n += sizeof(uint32_t);
}

static inline void pack_u32_t(uint32_t* x, unsigned char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((uint8_t*)(buff+*buff_n)) = *x;
    if(pack == false)   *x = *((uint32_t*)(buff+*buff_n));
    *buff_n += sizeof(uint32_t);
}

static inline void pack_16(int* x, unsigned char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((int16_t*)(buff+*buff_n)) = *x; 
    if(pack == false)   *x = *((int16_t*)(buff+*buff_n));
    *buff_n += sizeof(int16_t);
}

static inline void pack_u16(int* x, unsigned char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((uint16_t*)(buff+*buff_n)) = *x; 
    if(pack == false)   *x = *((uint16_t*)(buff+*buff_n));
    *buff_n += sizeof(uint16_t);
}

static inline void pack_8(int* x, unsigned char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((int8_t*)(buff+*buff_n)) = *x; 
    if(pack == false)   *x = *((int8_t*)(buff+*buff_n));
    *buff_n += sizeof(int8_t);
}

static inline void pack_u8(int* x, unsigned char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((uint8_t*)(buff+*buff_n)) = *x; 
    if(pack == false)   *x = *((uint8_t*)(buff+*buff_n));
    *buff_n += sizeof(uint8_t);
}


typedef void (*pt2handler)(unsigned char*, int, int*);

class FixedSizeNetPacketToServer {
    public:

        void serialize(unsigned char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            packet(buff, buff_n, true);
            *size = *buff_n - _buff_n;
        };
        void unserialize(unsigned char* buff, int* buff_n, int* size){
            int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        virtual inline void packet(unsigned char* buff, int* buff_n, bool pack) = 0;

        void send() {
            unsigned char* buff= NetClient::get_client_out_buffer();
            int* buff_n = NetClient::get_client_out_buffer_n();
            if(*buff_n > 800) { printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n); return; }

            int bytes_written;
            serialize(buff, buff_n, &bytes_written);
        }

        void register_handler(pt2handler* handler) {
            return; //implement!
        }

};

class FixedSizeNetPacketToClient {
    public:
        void serialize(unsigned char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            packet(buff, buff_n, true);
            *size = *buff_n - _buff_n;
        };
        void unserialize(unsigned char* buff, int* buff_n, int* size){
            int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        virtual inline void packet(unsigned char* buff, int* buff_n, bool pack) = 0;

        void sendToClient(int client_id) {
            unsigned char buff[64]; //max message size
            int buff_n = 0;
            int size;
            serialize(buff, &buff_n, &size);
            push_message(client_id, buff, size);
        }

        void broadcast() {
            unsigned char buff[64]; //max message size
            int buff_n = 0;
            int size;
            serialize(buff, &buff_n, &size);
            push_broadcast_message(buff, size);
        }
};
/*
EXAMPLE
*/



//helper class

class ClientToServer {
    public:
        void send();
        void register_handler(pt2handler* handler);
        virtual void serialize(unsigned char* buff, int* buff_n, int* size){};
        //virtual void serialize(unsigned char* buff, int* buff_n, int* size) = 0;
};

/*
void ClientToServer::serialize(unsigned char* buff, int* buff_n, int* size) {
    printf("ClientToServer: this should never happen\n");
    return;
}
*/


void ClientToServer::send() {
    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();
    if(*buff_n > 800) { printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n); return; }

    int bytes_written;
    serialize(buff, buff_n, &bytes_written);
}

//void push_message(int client_id, unsigned char* buffer, int n_bytes); //que a message for client
//void push_broadcast_message(unsigned char* buffer, int n_bytes);  //que a message for broadcast to all clients

void ClientToServer::register_handler(pt2handler* handler) {
    
}


class ServerToClient {
    public:
        void sendToClient(int client_id);  
        void broadcast();
        void register_handler(pt2handler* handler);
        virtual void serialize(unsigned char* buff, int* buff_n, int* size) =0;
};

void ServerToClient::sendToClient(int client_id) {
    unsigned char buff[64]; //max message size
    int buff_n = 0;
    int size;
    serialize(buff, &buff_n, &size);
    push_message(client_id, buff, size);
}

void ServerToClient::broadcast() {
    unsigned char buff[64]; //max message size
    int buff_n = 0;
    int size;
    serialize(buff, &buff_n, &size);
    push_broadcast_message(buff, size);
}

/*
void ServerToClient::serialize(unsigned char* buff, int* buff_n, int* size) {
    printf("ServerToClient: this should never happen\n");
    return;
}
*/

/*
    agent control state message
*/

/*
/// message handling
void handle_agent_control_state_message(unsigned char* buff, int buff_n, int* read_bytes) {
    Agent_control_state_message cs;
    cs.deserialize(buff, buff_n, read_bytes);
}
*/


class Agent_control_state_message: public FixedSizeNetPacketToServer
{
    public:
        static const int message_id = 3;
        static const int size = 2*sizeof(uint8_t) + 2*sizeof(uint16_t) + 3*sizeof(uint32_t);

        int id;
        int seq;
        uint32_t cs;
        int tick;
        int ctick;
        float theta;
        float phi;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_message_id(message_id, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_16(&tick, buff, buff_n, pack);
            pack_u32_t(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }
};

/*
class Agent_control_state_message: public ClientToServer 
{
    public:
        static const int message_id = 3;
        static const int size = 2*sizeof(uint8_t) + 2*sizeof(uint16_t) + 3*sizeof(uint32_t);

        int id;
        int seq;
        uint32_t cs;
        unsigned int tick;
        unsigned int ctick;
        float theta;
        float phi;

        void serialize(unsigned char* buff, int* buff_n, int* bytes_written);
        void deserialize(unsigned char* buff, int buff_n, int* read_bytes);
};
*/

/*
void Agent_control_state_message::serialize(unsigned char* buff, int* buff_n, int* bytes_written) {

    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();

    if(*buff_n > 800) {
        printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n);
        return;
    }

    int _buff_n = *buff_n;
    PACK_uint8_t(this->message_id, buff, buff_n);  //push message id on stack
    PACK_uint16_t(id, buff, buff_n); //agent id
    PACK_uint8_t(seq, buff, buff_n);
    PACK_uint16_t(tick%65536, buff, buff_n);
    PACK_uint32_t(cs, buff, buff_n);
    PACK_float(theta, buff, buff_n);
    PACK_float(phi, buff, buff_n);
    *bytes_written= *buff_n-_buff_n;
}

void Agent_control_state_message::deserialize(unsigned char* buff, int buff_n, int* read_bytes) {
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

*/
/*
    agent state message
*/

class Agent_state_message: public ServerToClient
{
    public:
        static const int message_id = 4;
        static const int size = 2*sizeof(uint8_t)+2*sizeof(uint16_t)+6*sizeof(uint32_t);

        int id;
        int seq;
        int tick;

        float x;
        float y;
        float z;
        float vx,vy,vz;

        void serialize(unsigned char* buff, int* buff_n, int* bytes_written);
        void deserialize(unsigned char* buff, int buff_n, int* read_bytes);
};

/*
#define Agent_state_message_id 4
#define Agent_state_message_size 2*sizeof(uint8_t)+2*sizeof(uint16_t)+6*sizeof(uint32_t)

void handle_agent_state_message(unsigned char* buff, int buff_n, int* bytes_read) {
    Agent_state_message cs;
    cs.deserialize(buff, buff_n, bytes_read); //should pass in pointer for number of ints read

    printf("Agent state received: id=%i, seq=%i \n", cs.id, cs.seq);

    //check id, if id does not exist, then create agent on receipt
}
*/

/*
typedef void (*pt2handler)(unsigned char*, int, int*);
void Agent_state_message_register_message() {
    register_message_handler(this->id, this->size, (pt2handler) &handle_agent_control_state_message);
    printf("Registering handler for agent control state input: message_id= %i, message_size= %i\n", Agent_state_message_id, Agent_state_message_size);
}
*/

/*
!!! Need to be able to broadcast messages


*/

void Agent_state_message::serialize(unsigned char* buff, int* buff_n, int* bytes_written) {

/*
    unsigned char* buff= NetClient::get_client_out_buffer();
    int* buff_n = NetClient::get_client_out_buffer_n();

    int bcount = *buff_n;
    if(*buff_n > 800) {
        printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n);
        return;
    }
*/
    PACK_uint8_t(this->message_id, buff, buff_n);  //push message id on stack
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
    tick =UPACK_uint16_t(buff, &buff_n);

    x = UPACK_float(buff, &buff_n);
    y = UPACK_float(buff, &buff_n);
    z = UPACK_float(buff, &buff_n);
    vx = UPACK_float(buff, &buff_n);
    vy = UPACK_float(buff, &buff_n);
    vz = UPACK_float(buff, &buff_n);

    *read_bytes = buff_n - _buff_n;
}
