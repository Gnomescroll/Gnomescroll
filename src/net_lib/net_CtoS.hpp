#pragma once

#include <net_lib/global.hpp>

#include <net_lib/common/type_pack.h>   //REMOVE FROM INCLUDE
#include <net_lib/common/packet_buffer.hpp> //REMOVE THIS FROM INCLUDE!!!

#include <net_lib/common/packet_id_counter.hpp>

//#define NET_PERF1_DISABLED 1 //performance enhancement by amortizing serialization
/*
    !!!

    Note:  Currently, netmessages are replicated multiple times on server
    , even if they will be sent to multiple clients


    Because it introduces bugs, the bunching of netmessages was deprecated.

    Needs a new interface
    !!!
*/

/*
    TODO:
    have a "start" and "fast send" function for server to client packets sent to multiple clients
*/

template <class Derived>
class FixedSizeNetPacketToServer {
    private:
        virtual void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0;
    public:
        static uint8_t message_id;
        static int size;
        int client_id; //id of the UDP client who sent message

        //flatten this
        void serialize(char* buff, int* buff_n) __attribute((always_inline))
        {
            //GS_ASSERT(Derived::message_id != 255);
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
        }
        //flatten this
        inline void unserialize(char* buff, int* buff_n, int* size) __attribute((always_inline))
        {
            int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        void send() 
        {
            Net_message* nm = Net_message::acquire(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::Server.push_unreliable_message(nm);
        }
        
        //will overflow if more than 128 bytes
        int _size()
        {
            char buff[128];
            int buff_n = 0;
            int size = 0;
            unserialize(buff, &buff_n, &size);
            size++; // add a byte for the message id
            GS_ASSERT(size > 0 && size < 128);
            //printf("ToServer: %2d,%2d\n", message_id, size);
            return size;
        }

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            x.client_id = _client_id;   //client id of client who sent the packet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_server_packet() {
            Derived x = Derived();
            Derived::message_id = next_server_packet_id(); //set size
            //GS_ASSERT(Derived::message_id != 255);
            Derived::size = x._size();
            register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }

};

//template <typename T> int Base<T>::staticVar(0);
template <class Derived> uint8_t FixedSizeNetPacketToServer<Derived>::message_id(255);
template <class Derived> int FixedSizeNetPacketToServer<Derived>::size(-1);



template <class Derived>
class FixedSizeReliableNetPacketToServer {

    private:
        virtual void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0;
    public:
        static uint8_t message_id;
        static int size;
        int client_id; //id of the UDP client who sent message

        void serialize(char* buff, int* buff_n) __attribute((always_inline))
        {
            //GS_ASSERT(Derived::message_id != 255);
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
        }
        inline void unserialize(char* buff, int* buff_n, int* size) __attribute((always_inline))
        {
            int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        void send() 
        {
            Net_message* nm = Net_message::acquire(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::Server.push_reliable_message(nm);
        }
        
        //will overflow if more than 128 bytes
        int _size()
        {
            char buff[128];
            int buff_n = 0;
            int size = 0;
            unserialize(buff, &buff_n, &size);
            size++; // add a byte for the message id
            GS_ASSERT(size > 0 && size < 128);
            //printf("ReliableToServer: %2d,%2d\n", message_id, size);
            return size;
        }

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            x.client_id = _client_id;   //client id of client who sent the packet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_server_packet() {
            Derived x = Derived();
            Derived::message_id = next_server_packet_id(); //set size
            //GS_ASSERT(Derived::message_id != 255);
            Derived::size = x._size();
            register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }

};

//template <typename T> int Base<T>::staticVar(0);
template <class Derived> uint8_t FixedSizeReliableNetPacketToServer<Derived>::message_id(255);
template <class Derived> int FixedSizeReliableNetPacketToServer<Derived>::size(-1);

