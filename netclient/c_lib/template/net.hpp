#pragma once

#include <net_lib/client/client.hpp>
#include <net_lib/server/server.h>

#include <net_lib/common/packet_buffer.hpp>

static int _packet_id = 1;
int next_packet_id() { return _packet_id++; }

typedef void (*pt2handler)(char*, int, int*, int);

template <class Derived>
class FixedSizeNetPacketToServer {
    private:
        virtual inline void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0;
    public:
        static int message_id;
        static int size;
        int client_id; //id of the UDP client who sent message

        void serialize(char* buff, int* buff_n) { //, int* size
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, true);
            packet(buff, buff_n, true);
            //*size = *buff_n - _buff_n;
        }
        inline void unserialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, false); //auto message unpack
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        void send() {
            Net_message* nm = Net_message::acquire_unreliable(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::NPserver.push_unreliable_packet(nm);
            //if(bytes_written != Derived::size ) printf("Error: message serialization size wrong\n"); //DEBUG
        }
        

        //will overflow if more than 64 bytes
        int Size() { char buff[64];int buff_n = 0;int _s;unserialize(buff, &buff_n, &_s);return _s;}

        //virtual inline void handle() = 0;

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            x.client_id = _client_id;   //client id of client who sent the packet
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
    private:
        virtual inline void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0 ;

    public:
        static int message_id;
        static int size;
        //int client_id; //not used yet

        void serialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, true);
            packet(buff, buff_n, true);
            *size = *buff_n - _buff_n;
        }
        inline void unserialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, false); //auto message unpack
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }

        void sendToClient(int client_id) {
            char buff[64]; //max message size
            int buff_n = 0;
            int size;
            serialize(buff, &buff_n, &size);
            push_message(client_id, buff, size);
        }

        void broadcast() {
            char buff[64]; //max message size
            int buff_n = 0;
            int size;
            serialize(buff, &buff_n, &size);
            push_broadcast_message(buff, size);
        }

        //will overflow if more than 64 bytes
        int _size() { char buff[64];int buff_n = 0;int size;serialize(buff, &buff_n, &size);return size;}

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            //x.client_id = _client_id //not used yet
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

//reliable packets

template <class Derived>
class FixedSizeReliableNetPacketToServer {
    private:
        virtual inline void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0 ;

    public:
        static int message_id;
        static int size;
        int client_id; //id of the UDP client who sent message

        void serialize(char* buff, int* buff_n) { //, int* size
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, true);
            packet(buff, buff_n, true);
            //*size = *buff_n - _buff_n;
        }
        inline void unserialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, false); //auto message unpack
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        void send() {
            Net_message* nm = Net_message::acquire_unreliable(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::NPserver.push_unreliable_packet(nm);
            //if(bytes_written != Derived::size ) printf("Error: message serialization size wrong\n"); //DEBUG
        }
        

        //will overflow if more than 64 bytes
        int Size() { char buff[64];int buff_n = 0;int _s;unserialize(buff, &buff_n, &_s);return _s;}

        //virtual inline void handle() = 0;

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            x.client_id = _client_id;   //client id of client who sent the packet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_reliable_server_packet() {
            Derived x = Derived();
            Derived::message_id = next_packet_id(); //set size
            Derived::size = x.Size();
            register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }

};

//template <typename T> int Base<T>::staticVar(0);
template <class Derived> int FixedSizeReliableNetPacketToServer<Derived>::message_id(255);
template <class Derived> int FixedSizeReliableNetPacketToServer<Derived>::size(-1);


template <class Derived>
class FixedSizeReliableNetPacketToClient {
    private:
        virtual inline void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0 ;
    public:
        static int message_id;
        static int size;
        //int client_id; //not used yet

        void serialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, true);
            packet(buff, buff_n, true);
            *size = *buff_n - _buff_n;
        }
        inline void unserialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n, false); //auto message unpack
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }

        void sendToClient(int client_id) {
            char buff[64]; //max message size
            int buff_n = 0;
            int size;
            serialize(buff, &buff_n, &size);
            push_message(client_id, buff, size);
        }

        void broadcast() {
            char buff[64]; //max message size
            int buff_n = 0;
            int size;
            serialize(buff, &buff_n, &size);
            push_broadcast_message(buff, size);
        }

        //will overflow if more than 64 bytes
        int _size() { char buff[64];int buff_n = 0;int size;serialize(buff, &buff_n, &size);return size;}

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            //x.client_id = _client_id //not used yet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_reliable_client_packet() {
            Derived x = Derived();
            Derived::message_id = next_packet_id(); //set size
            x.size = x._size();
            register_client_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }
}; 

template <class Derived> int FixedSizeReliableNetPacketToClient<Derived>::message_id(255);
template <class Derived> int FixedSizeReliableNetPacketToClient<Derived>::size(-1);