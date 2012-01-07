#include "net.hpp"

/*
static int _server_packet_id = 1;
int next_server_packet_id() { return _server_packet_id++; }

static int _client_packet_id = 1;
int next_client_packet_id() { return _client_packet_id++; }

typedef void (*pt2handler)(char*, int, int*, int);
*/


template <class Derived>
void FixedSizeNetPacketToServer<Derived>::serialize(char* buff, int* buff_n) 
{ 
    //, int* size
    //int _buff_n = *buff_n;
    pack_message_id(Derived::message_id, buff, buff_n, true);
    packet(buff, buff_n, true);
    //*size = *buff_n - _buff_n;
}

template <class Derived>     
void FixedSizeNetPacketToServer<Derived>::unserialize(char* buff, int* buff_n, int* size) 
{
    int _buff_n = *buff_n;
    pack_message_id(Derived::message_id, buff, buff_n, false); //auto message unpack
    packet(buff, buff_n, false);
    *size = *buff_n - _buff_n;
}

template <class Derived>         
void FixedSizeNetPacketToServer<Derived>::send() 
{
    Net_message* nm = Net_message::acquire(Derived::size);
    int buff_n = 0;
    serialize(nm->buff, &buff_n);
    NetClient::Server.push_unreliable_message(nm);
    //if(bytes_written != Derived::size ) printf("Error: message serialization size wrong\n"); //DEBUG
}
        
template <class Derived>    
int FixedSizeNetPacketToServer<Derived>::get_size() { char buff[128];int buff_n = 0;int _s;unserialize(buff, &buff_n, &_s);return _s;}

//static
template <class Derived>    
void FixedSizeNetPacketToServer<Derived>::handler(char* buff, int buff_n, int* bytes_read, int _client_id) 
{
    Derived x;  //allocated on stack
    x.client_id = _client_id;   //client id of client who sent the packet
    x.unserialize(buff, &buff_n, bytes_read);
    x.handle();
}

//static
template <class Derived>    
void FixedSizeNetPacketToServer<Derived>::register_server_packet() 
{
    Derived x = Derived();
    Derived::message_id = next_server_packet_id(); //set size
    Derived::size = x.get_size();
    register_server_message_handler(Derived::message_id, Derived::size, &Derived::handler);   //server/client handler
}



//template <typename T> int Base<T>::staticVar(0);
