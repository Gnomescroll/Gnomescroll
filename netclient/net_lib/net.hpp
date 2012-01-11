#pragma once

#include <net_lib/global.hpp>

#include <net_lib/common/type_pack.h>
#include <net_lib/common/packet_buffer.hpp>

#define NET_PERF1_DISABLED 1 //performance enhancement by amortizing serialization
/*
    When this is set to zero, each packet only needs to be serialized/allocated once even if sent to multiple clients
    This causes issues with staticly allocated packet, unless you set nm to NULL before using
*/

/*
    TODO:
    have a "start" and "fast send" function for server to client packets sent to multiple clients
*/

static int _server_packet_id = 1;
int next_server_packet_id() { return _server_packet_id++; }

static int _client_packet_id = 1;
int next_client_packet_id() { return _client_packet_id++; }

template <class Derived>
class FixedSizeNetPacketToServer {
    private:
        virtual void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0;
    public:
        static int message_id;
        static int size;
        int client_id; //id of the UDP client who sent message

        //flatten this
        void serialize(char* buff, int* buff_n) { //, int* size
            //int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
            //*size = *buff_n - _buff_n;
        }
        //flatten this
        inline void unserialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        void send() {
            Net_message* nm = Net_message::acquire(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::Server.push_unreliable_message(nm);
            //if(bytes_written != Derived::size ) printf("Error: message serialization size wrong\n"); //DEBUG
        }
        

        //will overflow if more than 64 bytes
        int Size() { char buff[128];int buff_n = 0;int _s;unserialize(buff, &buff_n, &_s);return _s+1;}

        //virtual inline void handle() = 0;

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            x.client_id = _client_id;   //client id of client who sent the packet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_server_packet() {
            Derived x = Derived();
            Derived::message_id = next_server_packet_id(); //set size
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
        virtual void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0 ;
        class Net_message* nm;
    public:
        static int message_id;
        static int size;
        //int client_id; //not used yet

        FixedSizeNetPacketToClient() { nm = NULL; }
        
        void serialize(char* buff, int* buff_n) { //, int* size
            //int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
            //*size = *buff_n - _buff_n;
        }
        inline void unserialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }

        /*
            For higher performance, require explicit serialization
        */
        void sendToClient(int client_id) {
            /*
            char buff[128]; //max message size
            int buff_n = 0;
            int size;
            serialize(buff, &buff_n, &size);
            push_message(client_id, buff, size);
            */

            //this will segfault
            
            if(nm == NULL || NET_PERF1_DISABLED ) 
            {
                nm = Net_message::acquire(Derived::size);
                int buff_n = 0;
                serialize(nm->buff, &buff_n);
            }
            
        /*
            Net_message* nm = Net_message::acquire(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);
            //NetClient::Server.push_unreliable_message(nm);
        */     
            if(NetServer::pool[client_id] == NULL)
            {
                printf("FixedSizeNetPacketToClient: sendToClient error, client_id %i is null\n", client_id);
                return;
            }
            NetServer::pool[client_id]->push_unreliable_message(nm);
        }

        void broadcast() {
            /*
            char buff[128]; //max message size
            int buff_n = 0;
            int size;
            serialize(buff, &buff_n, &size);
            push_broadcast_message(buff, size);
            */
            Net_message* nm = Net_message::acquire(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);

            class NetPeer* np;

            for(int i=0; i<NetServer::HARD_MAX_CONNECTIONS; i++) 
            {
                np = NetServer::pool[i]; //use better iterator
                if(np == NULL) continue;
                np->push_unreliable_message(nm);
            }
        }

        //will overflow if more than 64 bytes
        int _size() { char buff[128];int buff_n = 0;int size;unserialize(buff, &buff_n, &size);return size+1;}

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            //x.client_id = _client_id //not used yet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_client_packet() {
            Derived x = Derived();
            Derived::message_id = next_client_packet_id(); //set size
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
        virtual void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0;
    public:
        static int message_id;
        static int size;
        int client_id; //id of the UDP client who sent message

        void serialize(char* buff, int* buff_n) { //, int* size
            //int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
            //*size = *buff_n - _buff_n;
        }
        inline void unserialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        void send() {
            /*
            Net_message* nm = Net_message::acquire(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::Server.push_unreliable_message(nm);
            */
            Net_message* nm = Net_message::acquire(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::Server.push_reliable_message(nm);
        }
        

        //will overflow if more than 64 bytes
        int Size() { char buff[128];int buff_n = 0;int _s;unserialize(buff, &buff_n, &_s);return _s+1;}

        //virtual inline void handle() = 0;

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            x.client_id = _client_id;   //client id of client who sent the packet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_server_packet() {
            Derived x = Derived();
            Derived::message_id = next_server_packet_id(); //set size
            Derived::size = x.Size();
            register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }

};

//template <typename T> int Base<T>::staticVar(0);
template <class Derived> int FixedSizeReliableNetPacketToServer<Derived>::message_id(255);
template <class Derived> int FixedSizeReliableNetPacketToServer<Derived>::size(-1);

/*
Optimize this so it only serilizes once when sending to multiple clients
Should onyl use one net message allocation per message
*/

template <class Derived>
class FixedSizeReliableNetPacketToClient {
    private:
        virtual void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0 ;
        class Net_message* nm;
    public:
        static int message_id;
        static int size;
        //int client_id; //not used yet

        //FixedSizeReliableNetPacketToClient() : nm(NULL) {}
        FixedSizeReliableNetPacketToClient(){ nm = NULL; }

        void serialize(char* buff, int* buff_n) { //, int* size
            //int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
            //*size = *buff_n - _buff_n;
        }
        inline void unserialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }

        void sendToClient(int client_id) {
            /*
            char buff[128]; //max message size
            int buff_n = 0;
            int size;
            serialize(buff, &buff_n, &size);
            push_message(client_id, buff, size);
            */

            
            if(nm == NULL || NET_PERF1_DISABLED ) 
            {
                nm = Net_message::acquire(Derived::size);
                int buff_n = 0;
                serialize(nm->buff, &buff_n);
            }
            

            /*
            Net_message* nm = Net_message::acquire(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);
            */

            if(NetServer::pool[client_id] == NULL)
            {
                printf("FixedSizeReliableNetPacketToClient: sendToClient error, client_id %i is null\n", client_id);
                return;
            }
            NetServer::pool[client_id]->push_reliable_message(nm);
        }

        void broadcast() {
            /*
            char buff[128]; //max message size
            int buff_n = 0;
            int size;
            serialize(buff, &buff_n, &size);
            push_broadcast_message(buff, size);
            */
            Net_message* nm = Net_message::acquire(Derived::size);
            int buff_n = 0;
            serialize(nm->buff, &buff_n);

            class NetPeer* np;

            for(int i=0; i<NetServer::HARD_MAX_CONNECTIONS; i++) 
            {
                np = NetServer::pool[i]; //use better iterator
                if(np == NULL) continue;
                np->push_reliable_message(nm);
            }
        }

        int Size() { char buff[128];int buff_n = 0;int _s;unserialize(buff, &buff_n, &_s);return _s+1;}

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
            //x.client_id = _client_id //not used yet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_client_packet() {
            Derived x = Derived();
            Derived::message_id = next_client_packet_id(); //set size
            Derived::size = x.Size();
            register_client_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }
}; 

template <class Derived> int FixedSizeReliableNetPacketToClient<Derived>::message_id(255);
template <class Derived> int FixedSizeReliableNetPacketToClient<Derived>::size(-1);