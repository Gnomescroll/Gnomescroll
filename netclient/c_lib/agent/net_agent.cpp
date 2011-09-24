#pragma once

//#include "net_agent.hpp"

#include <net_lib/client/client.hpp>
#include <net_lib/common/message_handler.h>
#include <c_lib/agent/agent.hpp>
#include <net_lib/server/server.h>

//0 for unpack, 1 for pack


typedef void (*pt2handler)(unsigned char*, int, int*);

class FixedSizeNetPacketToServer {
    public:

        void serialize(unsigned char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            packet(buff, buff_n, true);
            *size = *buff_n - _buff_n;
        }
        void unserialize(unsigned char* buff, int* buff_n, int* size) {
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

        int size() { unsigned char buff[64];int buff_n = 0;int size;serialize(buff, &buff_n, &size);return size;}

        void register_server_packet_handler(pt2handler* handler) {
            return; //implement!
        }

};

class FixedSizeNetPacketToClient {
    public:
        void serialize(unsigned char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            packet(buff, buff_n, true);
            *size = *buff_n - _buff_n;
        }
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

        int size() { unsigned char buff[64];int buff_n = 0;int size;serialize(buff, &buff_n, &size);return size;}

        void register_client_packet_handler(pt2handler* handler) {
            //message_id = 5;
            return; //implement
        }
        virtual void handler(unsigned char* buff, int buff_n, int*);
};


class Agent_control_state_message: public FixedSizeNetPacketToServer
{
    public:
        static const int message_id = 3;
        //static const int size = 2*sizeof(uint8_t) + 2*sizeof(uint16_t) + 3*sizeof(uint32_t);

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



class Agent_state_message: public FixedSizeNetPacketToClient
{
    public:
        static const int message_id = 4;
        //static const int size = 2*sizeof(uint8_t)+2*sizeof(uint16_t)+6*sizeof(uint32_t);

        int id;
        int seq;
        int tick;

        float x;
        float y;
        float z;
        float vx,vy,vz;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_message_id(message_id, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_16(&tick, buff, buff_n, pack);

            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&vx, buff, buff_n, pack);
            pack_float(&vy, buff, buff_n, pack);
            pack_float(&vz, buff, buff_n, pack);
        }
};

//deprecate
/*
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

void Agent_state_message::serialize(unsigned char* buff, int* buff_n, int* bytes_written) {


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
*/