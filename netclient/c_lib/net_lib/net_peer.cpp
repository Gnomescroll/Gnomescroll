#include "./net_peer.hpp"

//#include <sys/types.h>

#define NET_PEER_DEBUG 1

NetPeer::NetPeer() {}


//class NetMessageManager reliable_message_manager;
//class NetMessageManager unreliable_message_manager;
//class NetMessageManager python_message_manager;


void NetPeer::push_reliable_message(class Net_message* nm)
{
    reliable_message_manager.push_message(nm);
}

void NetPeer::push_unreliable_packet(Net_message* nm) 
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


    int index;
    if(reliable_message_manager.pending_messages != 0) 
    {
        ENetPacket* reliable_p = enet_packet_create(NULL, reliable_message_manager.pending_bytes, ENET_PACKET_FLAG_RELIABLE);
        index = 0;
        reliable_message_manager.serialize_messages( reliable_p->data, 0);
    }

    if(unreliable_message_manager.pending_messages != 0) 
    {
        ENetPacket* unreliable_p = enet_packet_create(NULL, unreliable_message_manager.pending_bytes, ENET_PACKET_FLAG_RELIABLE);
        index = 0;
        unreliable_message_manager.serialize_messages( unreliable_p->data, 0);
    }

    if(python_message_manager.pending_messages != 0) 
    {
        ENetPacket* python_p = enet_packet_create(NULL, python_message_manager.pending_bytes, ENET_PACKET_FLAG_RELIABLE);
        index = 0;
        reliable_message_manager.serialize_messages( python_p->data, 0);
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
