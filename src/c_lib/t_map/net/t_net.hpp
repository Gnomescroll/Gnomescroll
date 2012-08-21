#pragma once

#include <net_lib/global.hpp>

#include <net_lib/common/type_pack.h>
#include <net_lib/common/packet_buffer.hpp>

#include <net_lib/common/packet_id_counter.hpp>
#include <net_lib/common/message_handler.h>

/*
    Map Message Channel

    client to server is over reliable channel
    server to client is over channel 3 (map channel)
*/

/*
    Just a reliable message to server
*/
template <class Derived>
class MapMessagePacketToServer {

    private:
        virtual void packet(char* buff, unsigned int* buff_n, bool pack) __attribute((always_inline)) = 0;
    public:
        static uint8_t message_id;
        static unsigned int size;
        int client_id; //id of the UDP client who sent message

        MapMessagePacketToServer() {}
        virtual ~MapMessagePacketToServer() {}
        
        void serialize(char* buff, unsigned int* buff_n) __attribute((always_inline))
        {
            pack_message_id(Derived::message_id, buff, buff_n);
            packet(buff, buff_n, true);
        }
        inline void unserialize(char* buff, unsigned int* buff_n, unsigned int* size) __attribute((always_inline))
        {
            unsigned int _buff_n = *buff_n;
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }
        
        void send() 
        {
            Net_message* nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);
            NetClient::Server.push_reliable_message(nm);
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
            //printf("MapMessageToServer: %2d,%2d\n", message_id, size);
            return size;
        }

        static void handler(char* buff, unsigned int buff_n, unsigned int* bytes_read, unsigned int _client_id) {
            Derived x;  //allocated on stack
            x.client_id = _client_id;   //client id of client who sent the packet
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_server_packet() {
            Derived x = Derived();
            Derived::message_id = next_server_packet_id(); //set size
            Derived::size = x._size();
            register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }

};

//template <typename T> int Base<T>::staticVar(0);
template <class Derived> uint8_t MapMessagePacketToServer<Derived>::message_id(255);
template <class Derived> unsigned int MapMessagePacketToServer<Derived>::size(0);



/*
    Special server packet
*/
template <class Derived>
class MapMessagePacketToClient {
    private:
        virtual void packet(char* buff, unsigned int* buff_n, bool pack) __attribute((always_inline)) = 0 ;
    public:
        static uint8_t message_id;
        static unsigned int size;

        MapMessagePacketToClient() { }
        virtual ~MapMessagePacketToClient() {}
    
        void serialize(char* buff, unsigned int* buff_n) __attribute((always_inline))
        {
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
            Deprecate This
        */
        void broadcast() 
        {
            if( NetServer::number_of_clients == 0) return; //prevents memory leak when no clients are connected

            Net_message* nm = Net_message::acquire(Derived::size);
            unsigned int buff_n = 0;
            serialize(nm->buff, &buff_n);

            class NetPeer* np;

            for(int i=0; i<NetServer::HARD_MAX_CONNECTIONS; i++) 
            {
                np = NetServer::pool[i]; //use better iterator
                if(np == NULL) continue;
                np->push_reliable_message(nm);
            }
        }

        void sendToClient(int client_id) 
        {
            NetPeer* np = NetServer::staging_pool[client_id];
            if (np == NULL) np = NetServer::pool[client_id];
            if(np == NULL)  //remove in debug
            {
                printf("FixedSizeReliableNetPacketToClient: sendToClient error, client_id %i is null. msg_id=%d \n", client_id, message_id);
                return;
            }

            if(np->map_message_buffer_index + size >= np->map_message_buffer_max) 
                np->resize_map_message_buffer(np->map_message_buffer_index + size);

            serialize(np->map_message_buffer, &np->map_message_buffer_index);
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
            //printf("MapMessageToClient: %2d,%2d\n", message_id, size);
            return size;
        }

        static void handler(char* buff, unsigned int buff_n, unsigned int* bytes_read, unsigned int _client_id) 
        {
            Derived x;
            x.unserialize(buff, &buff_n, bytes_read);
            x.handle();
        }

        static void register_client_packet() {
            Derived x = Derived();
            Derived::message_id = next_client_packet_id(); //set size
            Derived::size = x._size();
            register_client_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }
}; 

template <class Derived> uint8_t MapMessagePacketToClient<Derived>::message_id(255);
template <class Derived> unsigned int MapMessagePacketToClient<Derived>::size(0);

/*
    For sending Arrays of Messages

    -variable length messages
    -arrays of data
*/

/*
    Could be safer to size of buffer being read here
*/
template <class Derived>
class MapMessageArrayPacketToClient {
    private:
        virtual void packet(char* buff, unsigned int* buff_n, bool pack) __attribute((always_inline)) = 0 ;
    public:
        static uint8_t message_id;
        static unsigned int size;

        uint32_t byte_size;

        MapMessageArrayPacketToClient() {}
        virtual ~MapMessageArrayPacketToClient() {}

        void serialize(char* buff, unsigned int* buff_n) __attribute((always_inline))
        {
            pack_message_id(Derived::message_id, buff, buff_n);
            pack_u32(&byte_size, buff, buff_n, true);
            packet(buff, buff_n, true);
        }

        inline void unserialize(char* buff, unsigned int* buff_n, unsigned int* size) __attribute((always_inline))
        {
            unsigned int _buff_n = *buff_n;
            pack_u32(&byte_size, buff, buff_n, false);
            packet(buff, buff_n, false);
            *size = *buff_n - _buff_n;
        }

        void sendToClient(int client_id, char* buff, int len) 
        {
            NetPeer* np = NetServer::staging_pool[client_id];
            if (np == NULL) np = NetServer::pool[client_id];
            if(np == NULL)
            {
                printf("FixedSizeReliableNetPacketToClient: sendToClient error, client_id %i is null. msg_id=%d\n", client_id, message_id);
                return;
            }

            //printf("1 size= %i \n", size);
            //printf("2 size= %i \n", Derived::size);       
                
            if(np->map_message_buffer_index + size + len >= np->map_message_buffer_max) 
                np->resize_map_message_buffer(np->map_message_buffer_index + size + len);

            //if(len > 1024) 
            //{
                //printf("MapMessagePacketToClient: large map message, prefix length= %i length= %i \n", size, len);
            //}

            /*
                Have fast route for small messages
                For larger messages, force flush then construct packet
            */
            byte_size = len;
            serialize(np->map_message_buffer, &np->map_message_buffer_index);
            memcpy( np->map_message_buffer + np->map_message_buffer_index, buff, len);
            np->map_message_buffer_index += len;

            if(np->map_message_buffer_index >= 1024) np->flush_map_messages();
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
            //printf("MapMessageArrayToClient: %2d,%2d\n", message_id, size);
            return size;
        }

        static void handler(char* buff, unsigned int buff_n, unsigned int* bytes_read, unsigned int max_n) 
        {
            Derived x;

            //printf("1 read message: buff_n= %i bytes_read= %i \n", buff_n, *bytes_read);
            x._handle(buff, buff_n, bytes_read, max_n);
            //printf("2 read message: buff_n= %i bytes_read= %i \n", buff_n, *bytes_read);
        }

        virtual void handle(char* buff, int byte_num) __attribute((always_inline)) = 0;

        void _handle(char* buff, unsigned int buff_n, unsigned int* bytes_read, unsigned int max_n) __attribute((always_inline))
        {
            unserialize(buff, &buff_n, bytes_read);
            handle(buff+buff_n, byte_size);
            *bytes_read += byte_size;
        }

        static void register_client_packet() {
            Derived x = Derived();
            Derived::message_id = next_client_packet_id(); //set size
            Derived::size = x._size();
            register_client_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
        }
}; 

template <class Derived> uint8_t MapMessageArrayPacketToClient<Derived>::message_id(255);
template <class Derived> unsigned int MapMessageArrayPacketToClient<Derived>::size(0);
