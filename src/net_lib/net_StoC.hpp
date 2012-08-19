#pragma once

#include <net_lib/global.hpp>

#include <net_lib/common/type_pack.h>   //REMOVE FROM INCLUDE
#include <net_lib/common/packet_buffer.hpp> //REMOVE THIS FROM INCLUDE!!!

#include <net_lib/common/packet_id_counter.hpp>
#include <net_lib/common/message_handler.h>

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
class FixedSizeNetPacketToClient {
    private:
        virtual void packet(char* buff, unsigned int* buff_n, bool pack) __attribute((always_inline)) = 0 ;
        class Net_message* nm;
    public:
        static uint8_t message_id;
        static unsigned int size;
        static int _in;
        //int client_id; //not used yet

        FixedSizeNetPacketToClient() { nm = NULL; }
        virtual ~FixedSizeNetPacketToClient() {}
    
        void serialize(char* buff, unsigned int* buff_n) __attribute((always_inline))
        {
            //GS_ASSERT(Derived::message_id != 255);
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
        }

        inline void unserialize(char* buff, unsigned int* buff_n, unsigned int* size) __attribute((always_inline))
        {
            unsigned int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }

        /*
            For higher performance, require explicit serialization
        */
        void sendToClient(int client_id) 
        {
            //printf("%d Sending packet %d,%d to %d\n", _in++, message_id, size, client_id);

            nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);
  
            if (NetServer::staging_pool[client_id] == NULL && NetServer::pool[client_id] == NULL)
            {
                printf("FixedSizeNetPacketToClient: sendToClient error, client_id %i is null. msg_id=%d\n", client_id, message_id);
                return;
            }
            if (NetServer::staging_pool[client_id] != NULL)
                NetServer::staging_pool[client_id]->push_unreliable_message(nm);
            else
                NetServer::pool[client_id]->push_unreliable_message(nm);
        }

        void sendToClients(int* client_ids, unsigned int n_clients)
        {
            GS_ASSERT(client_ids != NULL);
            if (client_ids == NULL) return;
            GS_ASSERT(n_clients > 0);
            if (n_clients <= 0) return;

            GS_ASSERT(n_clients <= NetServer::number_of_clients);
            if (n_clients > NetServer::number_of_clients) return;

            Net_message* nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);

            int client_id;
            class NetPeer* np;
            for (unsigned int i=0; i<n_clients; i++) 
            {
                client_id = client_ids[i];
                GS_ASSERT(client_id >= 0 && client_id < NetServer::HARD_MAX_CONNECTIONS);
                if (client_id < 0 || client_id >= NetServer::HARD_MAX_CONNECTIONS) continue;
                np = NetServer::pool[client_id];
                GS_ASSERT(np != NULL);
                if (np == NULL) continue;
                np->push_unreliable_message(nm);
            }
        }

        void broadcast() 
        {
            if (NetServer::number_of_clients == 0) return; //prevents memory leak when no clients are connected

            //printf("%d Sending packet %d,%d\n", _in++, message_id, size);

            Net_message* nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);

            class NetPeer* np;

            for (int i=0; i<NetServer::HARD_MAX_CONNECTIONS; i++) 
            {
                np = NetServer::pool[i];
                if (np == NULL) continue;
                np->push_unreliable_message(nm);
            }
        }
        
        void broadcast_exclude(int ignore_client_id) 
        {
            if (NetServer::number_of_clients == 0) return; //prevents memory leak when no clients are connected

            //printf("%d Sending packet %d,%d\n", _in++, message_id, size);

            Net_message* nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);

            class NetPeer* np;

            for (int i=0; i<NetServer::HARD_MAX_CONNECTIONS; i++) 
            {
                if (i == ignore_client_id) continue;
                np = NetServer::pool[i];
                if (np == NULL) continue;
                np->push_unreliable_message(nm);
            }
        }

        //will overflow if more than 128 bytes
        unsigned int _size()
        {
            char buff[128];
            unsigned int buff_n = 0;
            unsigned int size = 0;
            unserialize(buff, &buff_n, &size);
            size++; // add a byte for the message id
            GS_ASSERT(size > 0 && size < 128);
            //printf("ToClient: %2d,%2d\n", message_id, size);
            return size;
        }

        static void handler(char* buff, unsigned int buff_n, unsigned int* bytes_read, unsigned int _client_id) 
        {
            Derived x;
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_client_packet() 
        {
            Derived x = Derived();
            Derived::message_id = next_client_packet_id(); //set size
            //GS_ASSERT(Derived::message_id != 255);
            x.size = x._size();
            register_client_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }
}; 

template <class Derived> uint8_t FixedSizeNetPacketToClient<Derived>::message_id(255);
template <class Derived> unsigned int FixedSizeNetPacketToClient<Derived>::size(0);
template <class Derived> int FixedSizeNetPacketToClient<Derived>::_in(0);

/*
Optimize this so it only serilizes once when sending to multiple clients
Should onyl use one net message allocation per message
*/

template <class Derived>
class FixedSizeReliableNetPacketToClient {
    private:
        virtual void packet(char* buff, unsigned int* buff_n, bool pack) __attribute((always_inline)) = 0 ;
        class Net_message* nm;
    public:
        static uint8_t message_id;
        static unsigned int size;

        static int _in;

        FixedSizeReliableNetPacketToClient(){ nm = NULL; }
        virtual ~FixedSizeReliableNetPacketToClient() {}
    
        void serialize(char* buff, unsigned int* buff_n) __attribute((always_inline))
        {
            //GS_ASSERT(Derived::message_id != 255);
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
        }
        inline void unserialize(char* buff, unsigned int* buff_n, unsigned int* size) __attribute((always_inline))
        {
            unsigned int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }

        void sendToClient(int client_id) 
        {
            //printf("%d Sending packet %d,%d to %d\n", _in++, message_id, size, client_id);
            nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);

            if (NetServer::staging_pool[client_id] == NULL && NetServer::pool[client_id] == NULL)
            {
                printf("FixedSizeReliableNetPacketToClient: sendToClient error, client_id %i is null. msg_id=%d\n", client_id, message_id);
                return;
            }
            if (NetServer::staging_pool[client_id] != NULL)
                NetServer::staging_pool[client_id]->push_reliable_message(nm);
            else
                NetServer::pool[client_id]->push_reliable_message(nm);
        }

        void sendToClients(int* client_ids, unsigned int n_clients)
        {
            GS_ASSERT(client_ids != NULL);
            if (client_ids == NULL) return;
            GS_ASSERT(n_clients > 0);
            if (n_clients <= 0) return;

            GS_ASSERT(n_clients <= NetServer::number_of_clients);
            if (n_clients > NetServer::number_of_clients) return;

            Net_message* nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);

            int client_id;
            class NetPeer* np;
            for (unsigned int i=0; i<n_clients; i++) 
            {
                client_id = client_ids[i];
                GS_ASSERT(client_id >= 0 && client_id < NetServer::HARD_MAX_CONNECTIONS);
                if (client_id < 0 || client_id >= NetServer::HARD_MAX_CONNECTIONS) continue;
                np = NetServer::pool[client_id];
                GS_ASSERT(np != NULL);
                if (np == NULL) continue;
                np->push_reliable_message(nm);
            }
        }

        void broadcast() 
        {
            if (NetServer::number_of_clients == 0) return;  //prevents memory leak when no clients are connected

            Net_message* nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);

            //printf("%d Sending packet %d,%d\n", _in++, message_id, size);

            for (int i=0; i<NetServer::HARD_MAX_CONNECTIONS; i++) 
            {
                class NetPeer* np = NetServer::pool[i];
                if (np == NULL) continue;
                np->push_reliable_message(nm);
            }
        }
        
        void broadcast_exclude(int ignore_client_id) 
        {
            if (NetServer::number_of_clients == 0) return;  //prevents memory leak when no clients are connected

            Net_message* nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);

            //printf("%d Sending packet %d,%d\n", _in++, message_id, size);

            for (int i=0; i<NetServer::HARD_MAX_CONNECTIONS; i++) 
            {
                if (i == ignore_client_id) continue;
                class NetPeer* np = NetServer::pool[i];
                if (np == NULL) continue;
                np->push_reliable_message(nm);
            }
        }

        //will overflow if more than 128 bytes
        unsigned int _size()
        {
            char buff[128];
            unsigned int buff_n = 0;
            unsigned int size = 0;
            unserialize(buff, &buff_n, &size);
            size++; // add a byte for the message id
            GS_ASSERT(size > 0 && size < 128);
            //printf("ReliableToClient: %2d,%2d\n", message_id, size);
            return size;
        }

        static void handler(char* buff, unsigned int buff_n, unsigned int* bytes_read, unsigned int _client_id) 
        {
            Derived x;
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_client_packet() 
        {
            Derived x = Derived();
            Derived::message_id = next_client_packet_id(); //set size
            Derived::size = x._size();
            register_client_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }
};

template <class Derived> uint8_t FixedSizeReliableNetPacketToClient<Derived>::message_id(255);
template <class Derived> unsigned int FixedSizeReliableNetPacketToClient<Derived>::size(0);
template <class Derived> int FixedSizeReliableNetPacketToClient<Derived>::_in(0);

