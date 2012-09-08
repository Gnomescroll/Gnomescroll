#include "./net_peer.hpp"

//#include <sys/types.h>

#define NET_PEER_DEBUG 0

NetPeer::NetPeer()
:
client_id(-1),
connected(0),
version(0),
disconnect_code(DISCONNECT_TIMEOUT),
enet_peer(NULL)
{
    #if DC_SERVER
    map_message_buffer = new char[ NET_PEER_MAP_MESSAGE_BUFFER_DEFAULT ];
    map_message_buffer_index = 0;
    map_message_buffer_max = NET_PEER_MAP_MESSAGE_BUFFER_DEFAULT;
    #else
    map_message_buffer = NULL;
    #endif
}

NetPeer::~NetPeer()
{
    delete[] map_message_buffer;
}


//class NetMessageManager reliable_message_manager;
//class NetMessageManager unreliable_message_manager;
//class NetMessageManager python_message_manager;


void NetPeer::push_reliable_message(class Net_message* nm)
{
    reliable_message_manager.push_message(nm);
}

void NetPeer::push_unreliable_message(Net_message* nm) 
{
    unreliable_message_manager.push_message(nm);
}

/*
void NetPeer::push_python_message(class Net_message* nm)
{
    python_message_manager.push_message(nm);
}
*/

void NetPeer::flush_map_messages()
{
    // TODO -- check if (this->kill)
    if (enet_peer == NULL) return;
    if (map_message_buffer_index == 0) return;
    //printf("Flushing %i map bytes \n", map_message_buffer_index);
    ENetPacket* map_p = enet_packet_create( map_message_buffer, map_message_buffer_index, ENET_PACKET_FLAG_RELIABLE);
    int ret = enet_peer_send (enet_peer, 3, map_p);
    if (ret < 0)
        enet_packet_destroy(map_p);
    map_message_buffer_index = 0;
}


void NetPeer::resize_map_message_buffer(unsigned int size_min)
{
    flush_map_messages();

    unsigned int size = 4096*((size_min / 4096) + 1); //round up to next 4096 bytes
    //printf("resize_ map message buffer from %i to %i \n", map_message_buffer_max, size);
    map_message_buffer_max = size;
    delete[] map_message_buffer;
    map_message_buffer = new char[map_message_buffer_max];
}


void NetPeer::flush_to_net() 
{
    if (this->connected == 0) return;
    if (reliable_message_manager.pending_bytes_out == 0) return;
    if (enet_peer == NULL) return;
    
    if (reliable_message_manager.pending_messages != 0) 
    {
        ENetPacket* reliable_p = enet_packet_create(NULL, reliable_message_manager.pending_bytes_out, ENET_PACKET_FLAG_RELIABLE);
        reliable_message_manager.serialize_messages( (char*) reliable_p->data, 0); //error
        int ret = enet_peer_send (enet_peer, 0, reliable_p);
        if (ret < 0)
            enet_packet_destroy(reliable_p);
    }

    if (unreliable_message_manager.pending_messages != 0) 
    {
        ENetPacket* unreliable_p = enet_packet_create(NULL, unreliable_message_manager.pending_bytes_out, ENET_PACKET_FLAG_RELIABLE);
        unreliable_message_manager.serialize_messages( (char*)unreliable_p->data, 0);
        int ret = enet_peer_send (enet_peer, 0, unreliable_p);
        if (ret < 0)
            enet_packet_destroy(unreliable_p);
    }

    #if DC_SERVER
    flush_map_messages();
    #endif
/*
    int n1 = 0;
    int seq = get_next_sequence_number(this);
*/

/*
    PACK_uint16_t(seq, net_out_buff, &n1); //sequence number

    PACK_uint16_t(get_sequence_number(this), net_out_buff, &n1); //max seq
    PACK_uint32_t(generate_outgoing_ack_flag(this), net_out_buff, &n1); //sequence number
*/

/*
    #if DC_CLIENT
    pviz_packet_sent(seq, n1);
    #endif
*/
}
