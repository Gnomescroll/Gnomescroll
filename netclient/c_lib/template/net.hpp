#pragma once

#include <net_lib/client/client.hpp>
#include <net_lib/server/server.h>

static int _packet_id = 1;
int next_packet_id() { return _packet_id++; }

typedef const void (*_pt2handler)(unsigned char*, int, int*);

typedef void (*pt2handler)(unsigned char*, int, int*);

template <class Derived>
class FixedSizeNetPacketToServer {
    public:
        static int message_id;
        static int size;

        void serialize(unsigned char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, true);
            packet(buff, buff_n, true);
            *size = *buff_n - _buff_n;
        }
        inline void unserialize(unsigned char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, false); //auto message unpack
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        virtual inline void packet(unsigned char* buff, int* buff_n, bool pack) = 0;
        //inline void packet(unsigned char* buff, int* buff_n, bool pack);

        void send() {
            unsigned char* buff= NetClient::get_client_out_buffer();
            int* buff_n = NetClient::get_client_out_buffer_n();
            if(*buff_n > 800) { printf("Cannot send message: output buffer is full! %i bytes\n", *buff_n); return; }
            int bytes_written;
            serialize(buff, buff_n, &bytes_written);
        }

        //will overflow if more than 64 bytes
        int Size() { unsigned char buff[64];int buff_n = 0;int _s;unserialize(buff, &buff_n, &_s);return _s;}

        //virtual inline void handle() = 0;

        static void handler(unsigned char* buff, int buff_n, int* bytes_read, int client_id) {
            Derived x;  //allocated on stack
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_server_packet() {
            Derived x = Derived();
            Derived::message_id = next_packet_id(); //set size
            Derived::size = x.Size();
            register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }

};

//template <typename T> int Base<T>::staticVar(0);
template <class Derived> int FixedSizeNetPacketToServer<Derived>::message_id(255);
template <class Derived> int FixedSizeNetPacketToServer<Derived>::size(-1);


template <class Derived>
class FixedSizeNetPacketToClient {
    public:
        static int message_id;
        static int size;

    public:
        void serialize(unsigned char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, true);
            packet(buff, buff_n, true);
            *size = *buff_n - _buff_n;
        }
        inline void unserialize(unsigned char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, false); //auto message unpack
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        virtual inline void packet(unsigned char* buff, int* buff_n, bool pack) = 0;
        //inline void packet(unsigned char* buff, int* buff_n, bool pack);

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

        //will overflow if more than 64 bytes
        int _size() { unsigned char buff[64];int buff_n = 0;int size;serialize(buff, &buff_n, &size);return size;}

        static void handler(unsigned char* buff, int buff_n, int* bytes_read, int client_id) {
            Derived x;  //allocated on stack
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_client_packet() {
            Derived x = Derived();
            Derived::message_id = next_packet_id(); //set size
            x.size = x._size();
            register_client_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }
}; 

template <class Derived> int FixedSizeNetPacketToClient<Derived>::message_id(255);
template <class Derived> int FixedSizeNetPacketToClient<Derived>::size(-1);
