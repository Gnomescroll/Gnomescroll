#include "packet_id_counter.hpp"

static uint8_t _server_packet_id = 0;
uint8_t next_server_packet_id()
{
    GS_ASSERT(_server_packet_id < 0xFF);
     return _server_packet_id++;
}

static uint8_t _client_packet_id = 0;
uint8_t next_client_packet_id()
{
    GS_ASSERT(_client_packet_id < 0xFF);
    return _client_packet_id++;
}
