#include "./net_peer.hpp"

//#include <sys/types.h>

#define NET_PEER_DEBUG 1

NetPeer::NetPeer() 
{
    connected = 0;
    client_id = -1;
    enet_peer = NULL;

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

void NetPeer::push_python_message(class Net_message* nm)
{
    python_message_manager.push_message(nm);
}


void NetPeer::flush_to_net() 
{
    if(this->connected == 0) 
    {
        printf("flush_outgoing_packets: Cannot send packet, disconnected!\n");
        return;
    }

    if(reliable_message_manager.pending_messages != 0) 
    {
        printf("%i = \n", reliable_message_manager.pending_bytes_out);
        ENetPacket* reliable_p = enet_packet_create(NULL, reliable_message_manager.pending_bytes_out, ENET_PACKET_FLAG_RELIABLE);
        reliable_message_manager.serialize_messages( (char*) reliable_p->data, 0);
        enet_peer_send (enet_peer, 0, reliable_p);
    }

    if(unreliable_message_manager.pending_messages != 0) 
    {
        ENetPacket* unreliable_p = enet_packet_create(NULL, unreliable_message_manager.pending_bytes_out, ENET_PACKET_FLAG_RELIABLE);
        unreliable_message_manager.serialize_messages( (char*)unreliable_p->data, 0);
        enet_peer_send (enet_peer, 0, unreliable_p);
    }

    if(python_message_manager.pending_messages != 0) 
    {
        ENetPacket* python_p = enet_packet_create(NULL, python_message_manager.pending_bytes_out, ENET_PACKET_FLAG_RELIABLE);
        reliable_message_manager.serialize_messages( (char*)python_p->data, 0);
        enet_peer_send (enet_peer, 2, python_p);
    }


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
    #ifdef DC_CLIENT
    pviz_packet_sent(seq, n1);
    #endif
*/
}
