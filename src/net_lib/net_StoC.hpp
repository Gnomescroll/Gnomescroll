#pragma once

#if DC_SERVER
# include <net_lib/server.hpp>
#endif

#include <net_lib/global.hpp>
#include <net_lib/common/packet_id_counter.hpp>
#include <net_lib/common/message_handler.h>
#include <net_lib/common/type_pack.hpp>

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
class FixedSizeNetPacketToClient
{
    private:
        class NetMessage* nm;
    virtual void packet(char* buff, size_t* buff_n, bool pack) = 0;

    public:
        static uint8_t message_id;
        static size_t size;
        static int _in;
        //ClientID client_id; //not used yet

    FixedSizeNetPacketToClient() :
        nm(NULL)
    {
    }

    virtual ~FixedSizeNetPacketToClient() {}

    ALWAYS_INLINE
    void serialize(char* buff, size_t* buff_n)
    {
        //GS_ASSERT(Derived::message_id != 255);
        pack_message_id(Derived::message_id, buff, buff_n);
        packet(buff, buff_n, true);
    }

    ALWAYS_INLINE
    void unserialize(char* buff, size_t* buff_n, size_t* size)
    {
        size_t _buff_n = *buff_n;
        packet(buff, buff_n, false);
        *size = *buff_n - _buff_n;
    }

    /*
        For higher performance, require explicit serialization
    */
    void sendToClient(ClientID client_id)
    {
        #if DC_SERVER
        nm = NetMessage::acquire(Derived::size);
        size_t buff_n = 0;
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
        #else
        GS_ASSERT(false);
        #endif
    }

    void sendToClients(ClientID* client_ids, size_t n_clients)
    {
        #if DC_SERVER
        GS_ASSERT(client_ids != NULL);
        if (client_ids == NULL) return;
        GS_ASSERT(n_clients > 0);
        if (n_clients <= 0) return;

        GS_ASSERT(n_clients <= NetServer::number_of_clients);
        if (n_clients > NetServer::number_of_clients) return;

        NetMessage* nm = NetMessage::acquire(Derived::size);
        size_t buff_n = 0;
        serialize(nm->buff, &buff_n);

        ClientID client_id;
        class NetPeer* np;
        for (size_t i=0; i<n_clients; i++)
        {
            client_id = client_ids[i];
            GS_ASSERT(client_id >= 0 && client_id < HARD_MAX_CONNECTIONS);
            if (client_id < 0 || client_id >= HARD_MAX_CONNECTIONS) continue;
            np = NetServer::pool[client_id];
            GS_ASSERT(np != NULL);
            if (np == NULL) continue;
            np->push_unreliable_message(nm);
        }
        #else
        GS_ASSERT(false);
        #endif
    }

    void broadcast()
    {
        #if DC_SERVER
        if (NetServer::number_of_clients == 0) return; //prevents memory leak when no clients are connected

        //printf("%d Sending packet %d,%d\n", _in++, message_id, size);

        NetMessage* nm = NetMessage::acquire(Derived::size);
        size_t buff_n = 0;
        serialize(nm->buff, &buff_n);

        class NetPeer* np;

        for (int i=0; i<HARD_MAX_CONNECTIONS; i++)
        {
            np = NetServer::pool[i];
            if (np == NULL) continue;
            np->push_unreliable_message(nm);
        }
        #else
        GS_ASSERT(false);
        #endif
    }

    void broadcast_exclude(int ignore_client_id)
    {
        #if DC_SERVER
        if (NetServer::number_of_clients == 0) return; //prevents memory leak when no clients are connected

        //printf("%d Sending packet %d,%d\n", _in++, message_id, size);

        NetMessage* nm = NetMessage::acquire(Derived::size);
        size_t buff_n = 0;
        serialize(nm->buff, &buff_n);

        class NetPeer* np;

        for (int i=0; i<HARD_MAX_CONNECTIONS; i++)
        {
            if (i == ignore_client_id) continue;
            np = NetServer::pool[i];
            if (np == NULL) continue;
            np->push_unreliable_message(nm);
        }
        #else
        GS_ASSERT(false);
        #endif
    }

    //will overflow if more than 128 bytes
    size_t _size()
    {
        char buff[128] = {0};
        size_t buff_n = 0;
        size_t size = 0;
        unserialize(buff, &buff_n, &size);
        size++; // add a byte for the message id
        GS_ASSERT(size > 0 && size < 128);
        //printf("ToClient: %2d,%2d\n", message_id, size);
        return size;
    }

    static void handler(char* buff, size_t buff_n, size_t* bytes_read)
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
template <class Derived> size_t FixedSizeNetPacketToClient<Derived>::size(0);
template <class Derived> int FixedSizeNetPacketToClient<Derived>::_in(0);

/*
Optimize this so it only serilizes once when sending to multiple clients
Should onyl use one net message allocation per message
*/

template <class Derived>
class FixedSizeReliableNetPacketToClient
{
    private:
        class NetMessage* nm;
    virtual void packet(char* buff, size_t* buff_n, bool pack) = 0;

    public:
        static uint8_t message_id;
        static size_t size;

        static int _in;

    FixedSizeReliableNetPacketToClient() :
        nm(NULL)
    {
    }

    virtual ~FixedSizeReliableNetPacketToClient() {}

    ALWAYS_INLINE void serialize(char* buff, size_t* buff_n)
    {
        //GS_ASSERT(Derived::message_id != 255);
        pack_message_id(Derived::message_id, buff, buff_n);
        packet(buff, buff_n, true);
    }

    ALWAYS_INLINE void unserialize(char* buff, size_t* buff_n, size_t* size)
    {
        size_t _buff_n = *buff_n;
        packet(buff, buff_n, false);
        *size = *buff_n - _buff_n;
    }

    void sendToClient(ClientID client_id)
    {
        #if DC_SERVER
        //printf("%d Sending packet %d,%d to %d\n", _in++, message_id, size, client_id);
        nm = NetMessage::acquire(Derived::size);
        size_t buff_n = 0;
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
        #else
        GS_ASSERT(false);
        #endif
    }

    void sendToClients(ClientID* client_ids, size_t n_clients)
    {
        #if DC_SERVER
        GS_ASSERT(client_ids != NULL);
        if (client_ids == NULL) return;
        GS_ASSERT(n_clients > 0);
        if (n_clients <= 0) return;

        GS_ASSERT(n_clients <= NetServer::number_of_clients);
        if (n_clients > NetServer::number_of_clients) return;

        NetMessage* nm = NetMessage::acquire(Derived::size);
        size_t buff_n = 0;
        serialize(nm->buff, &buff_n);

        ClientID client_id;
        class NetPeer* np;
        for (size_t i=0; i<n_clients; i++)
        {
            client_id = client_ids[i];
            GS_ASSERT(client_id >= 0 && client_id < HARD_MAX_CONNECTIONS);
            if (client_id < 0 || client_id >= HARD_MAX_CONNECTIONS) continue;
            np = NetServer::pool[client_id];
            GS_ASSERT(np != NULL);
            if (np == NULL) continue;
            np->push_reliable_message(nm);
        }
        #else
        GS_ASSERT(false);
        #endif
    }

    void broadcast()
    {
        #if DC_SERVER
        if (NetServer::number_of_clients == 0) return;  //prevents memory leak when no clients are connected

        NetMessage* nm = NetMessage::acquire(Derived::size);
        size_t buff_n = 0;
        serialize(nm->buff, &buff_n);

        //printf("%d Sending packet %d,%d\n", _in++, message_id, size);

        for (int i=0; i<HARD_MAX_CONNECTIONS; i++)
        {
            class NetPeer* np = NetServer::pool[i];
            if (np == NULL) continue;
            np->push_reliable_message(nm);
        }
        #else
        GS_ASSERT(false);
        #endif
    }

    void broadcast_exclude(int ignore_client_id)
    {
        #if DC_SERVER
        if (NetServer::number_of_clients == 0) return;  //prevents memory leak when no clients are connected

        NetMessage* nm = NetMessage::acquire(Derived::size);
        size_t buff_n = 0;
        serialize(nm->buff, &buff_n);

        //printf("%d Sending packet %d,%d\n", _in++, message_id, size);

        for (int i=0; i<HARD_MAX_CONNECTIONS; i++)
        {
            if (i == ignore_client_id) continue;
            class NetPeer* np = NetServer::pool[i];
            if (np == NULL) continue;
            np->push_reliable_message(nm);
        }
        #else
        GS_ASSERT(false);
        #endif
    }

    //will overflow if more than 128 bytes
    size_t _size()
    {
        char buff[128] = {0};
        size_t buff_n = 0;
        size_t size = 0;
        unserialize(buff, &buff_n, &size);
        size++; // add a byte for the message id
        GS_ASSERT(size > 0 && size < 128);
        //printf("ReliableToClient: %2d,%2d\n", message_id, size);
        return size;
    }

    static void handler(char* buff, size_t buff_n, size_t* bytes_read)
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
template <class Derived> size_t FixedSizeReliableNetPacketToClient<Derived>::size(0);
template <class Derived> int FixedSizeReliableNetPacketToClient<Derived>::_in(0);

