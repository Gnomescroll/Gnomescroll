#pragma once

#include <net_lib/global.hpp>
#include <net_lib/common/message_handler.h>
#include <net_lib/common/packet_id_counter.hpp>
#include <net_lib/common/type_pack.hpp>

#if DC_CLIENT
#include <net_lib/client.hpp>
#endif

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

#if DC_CLIENT
void send_bullshit_data()
{
    unsigned int size = 0;
    do
    {
        size = rand()&0xff;
    } while (size == 0);

    class Net_message* nm = arbitrary_acquire(size);
    GS_ASSERT(nm != NULL);
    if (nm == NULL) return;

    unsigned int buff_n = 0;
    unsigned int message_id = rand()&0xff;
    pack_message_id(message_id, nm->buff, &buff_n);
    for (unsigned int i=buff_n; i<size; i++)
        nm->buff[i] = rand()&0xff;
    NetClient::Server.push_unreliable_message(nm);
}
#endif

template <class Derived>
class FixedSizeNetPacketToServer
{
    private:
        virtual void packet(char* buff, unsigned int* buff_n, bool pack) = 0;
    public:
        static uint8_t message_id;
        static unsigned int size;
        ClientID client_id; //id of the UDP client who sent message
        static const bool auth_required = true; // override in Derived class to disable

        FixedSizeNetPacketToServer() {}
        virtual ~FixedSizeNetPacketToServer() {}

        //flatten this
        ALWAYS_INLINE
        void serialize(char* buff, unsigned int* buff_n)
        {
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
        }

        ALWAYS_INLINE
        void unserialize(char* buff, unsigned int* buff_n, unsigned int* size)
        {
            unsigned int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }

        void send()
        {
            #if DC_CLIENT
            Net_message* nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::Server.push_unreliable_message(nm);
            #else
            GS_ASSERT(false);
            #endif
        }

        //will overflow if more than 128 bytes
        unsigned int _size()
        {
            char buff[128] = {0};
            unsigned int buff_n = 0;
            unsigned int size = 0;
            unserialize(buff, &buff_n, &size);
            size++; // add a byte for the message id
            GS_ASSERT(size > 0 && size < 128);
            //printf("ToServer: %2d,%2d\n", message_id, size);
            return size;
        }

        static void handler(char* buff, unsigned int buff_n, unsigned int* bytes_read, ClientID client_id)
        {
            #if DC_SERVER
            Derived x;  //allocated on stack
            if (NetServer::clients[client_id] == NULL ||   // auth check
                (x.auth_required && !NetServer::clients[client_id]->authorized))
                return;
            x.client_id = client_id;   //client id of client who sent the packet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
            #else
            GS_ASSERT(false);
            #endif
        }

        static void register_server_packet()
        {
            Derived x = Derived();
            Derived::message_id = next_server_packet_id(); //set size
            Derived::size = x._size();
            register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }
};

//template <typename T> int Base<T>::staticVar(0);
template <class Derived> uint8_t FixedSizeNetPacketToServer<Derived>::message_id(255);
template <class Derived> unsigned int FixedSizeNetPacketToServer<Derived>::size(0);



template <class Derived>
class FixedSizeReliableNetPacketToServer
{
    private:
        virtual void packet(char* buff, unsigned int* buff_n, bool pack) = 0;
        //virtual ALWAYS_INLINE void packet(char* buff, unsigned int* buff_n, bool pack) { GS_ASSERT(false); }
    public:
        static uint8_t message_id;
        static unsigned int size;
        ClientID client_id; //id of the UDP client who sent message
        static const bool auth_required = true; // override in Derived class to disable

        FixedSizeReliableNetPacketToServer() {}
        virtual ~FixedSizeReliableNetPacketToServer() {}

        ALWAYS_INLINE
        void serialize(char* buff, unsigned int* buff_n)
        {
            //GS_ASSERT(Derived::message_id != 255);
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
        }

        ALWAYS_INLINE
        void unserialize(char* buff, unsigned int* buff_n, unsigned int* size)
        {
            unsigned int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }

        void send()
        {
            #if DC_CLIENT
            Net_message* nm = Net_message::acquire(Derived::size);
            GS_ASSERT(nm != NULL);
            if (nm == NULL) return;
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::Server.push_reliable_message(nm);
            #else
            GS_ASSERT(false);
            #endif
        }

        //will overflow if more than 128 bytes
        unsigned int _size()
        {
            char buff[128] = {0};
            unsigned int buff_n = 0;
            unsigned int size = 0;
            unserialize(buff, &buff_n, &size);
            size++; // add a byte for the message id
            GS_ASSERT(size > 0 && size < 128);
            //printf("ReliableToServer: %2d,%2d\n", message_id, size);
            return size;
        }

        static void handler(char* buff, unsigned int buff_n, unsigned int* bytes_read, ClientID client_id)
        {
            #if DC_SERVER
            Derived x;  //allocated on stack
            if (NetServer::clients[client_id] == NULL ||   // auth check
                (x.auth_required && !NetServer::clients[client_id]->authorized))
                return;
            x.client_id = client_id;   //client id of client who sent the packet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
            #else
            GS_ASSERT(false);
            #endif
        }

        static void register_server_packet()
        {
            Derived x = Derived();
            Derived::message_id = next_server_packet_id(); //set size
            //GS_ASSERT(Derived::message_id != 255);
            Derived::size = x._size();
            register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }

};

//template <typename T> int Base<T>::staticVar(0);
template <class Derived> uint8_t FixedSizeReliableNetPacketToServer<Derived>::message_id(255);
template <class Derived> unsigned int FixedSizeReliableNetPacketToServer<Derived>::size(0);

