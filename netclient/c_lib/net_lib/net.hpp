#pragma once

#include <globals.hpp>

#include <net_lib/message_handler.h>

static int _server_packet_id = 1;
int next_server_packet_id() { return _server_packet_id++; }

static int _client_packet_id = 1;
int next_client_packet_id() { return _client_packet_id++; }

typedef void (*pt2handler)(char*, int, int*, int);

template <class Derived>
class PacketToServer {
    private:
        virtual void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0;
    public:
        static int message_id;
        static int size;
        int client_id;
        void serialize(char* buff, int* buff_n) {
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
        }
        inline void unserialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            //unpack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        void send() 
        {
            //int buff_n = client.bn;
        
            if(client.connected == 0) 
            {
                printf("Send failed, client not connected \n");
                return;
            }
            if(client.bn > 1500) client.flush_to_net();

            //printf("1 message %i: client.bn = %i \n", Derived::message_id, client.bn);
            serialize(client.buffer, &client.bn);
            //printf("2 message %i: client.bn = %i \n", Derived::message_id, client.bn);
        }

        int Size() { char buff[64];int buff_n = 0;int _s;unserialize(buff, &buff_n, &_s);return _s;}

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;
            x.client_id = _client_id;
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_server_packet() {
            Derived x = Derived();
            Derived::message_id = next_server_packet_id();
            Derived::size = x.Size();
            register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);
        }

};

//template <typename T> int Base<T>::staticVar(0);
template <class Derived> int PacketToServer<Derived>::message_id(255);
template <class Derived> int PacketToServer<Derived>::size(-1);

template <class Derived>
class PacketToClient {
    private:
        virtual void packet(char* buff, int* buff_n, bool pack) __attribute((always_inline)) = 0 ;
    public:
        static int message_id;
        static int size;

        void serialize(char* buff, int* buff_n) {
            //int _buff_n = *buff_n;
            pack_message_id(Derived::message_id, buff, buff_n);

            //if(*buff_n == _buff_n) printf("serialization ERROR\n");
            packet(buff, buff_n, true);
        }
        inline void unserialize(char* buff, int* buff_n, int* size) {
            int _buff_n = *buff_n;
            //unpack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }

        void sendToClient(int client_id) 
        {
            NetPeer* np = NetPeerList[client_id];

            if(np == NULL)
            {
                printf("Cannot send message: client %i is null \n", client_id);
                return;
            }
            if(np->bn > 1500)
            {
                printf("sendToClient: force flush: %i \n", np->bn);
                np->flush_to_net();
            }
            serialize(np->buffer, &np->bn);
        }

        void broadcast() {

            for(int i=0; i < client_limit; i++)
            {
                if(NetPeerList[i] != NULL)
                {
                    if(NetPeerList[i]->bn > 1450) NetPeerList[i]->flush_to_net();
                    serialize(NetPeerList[i]->buffer, &NetPeerList[i]->bn);
                }
            }
        }

        //int _size() { char buff[64];int buff_n = 0;int size;unserialize(buff, &buff_n, &size);return size;}
        int Size() { char buff[64];int buff_n = 0;int _s;unserialize(buff, &buff_n, &_s);return _s;}

        static void handler(char* buff, int buff_n, int* bytes_read, int _client_id) {
            Derived x;  //allocated on stack
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

template <class Derived> int PacketToClient<Derived>::message_id(255);
template <class Derived> int PacketToClient<Derived>::size(-1);