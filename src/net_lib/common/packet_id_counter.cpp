#include "packet_id_counter.hpp"

static int _server_packet_id = 0;
int next_server_packet_id()
{ 
     return _server_packet_id++; 
}

static int _client_packet_id = 0;
int next_client_packet_id() 
{
    return _client_packet_id++;
}
