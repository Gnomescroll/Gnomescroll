#include "./net_peer.hpp"

#include <errno.h>
#include <sys/types.h>

#include <globals.hpp>

#include <net_lib/message_handler.h>

char receive_buffer[4500];

void NetPeer::receive_packets()
{
    return; 
    if(connected == 0 )
    {
        printf("socket closed\n");
        return;
    }

    //int bytes_read =  recv(socket, receive_buffer, 4500, MSG_PEEK);

    uint16_t client_id;
    uint16_t packet_number;
    uint16_t packet_size;

    int n1 = 0;
    UNPACK_uint16_t(&client_id, receive_buffer, &n1); //client id
    UNPACK_uint16_t(&packet_number, receive_buffer, &n1); //packet_number
    UNPACK_uint16_t(&packet_size, receive_buffer, &n1); //packet_number

    if(packet_size < 6)
    {
        printf("WTF TCP ERROR 2 \n");
        return;
    }

    int bytes_read = 0;

    if(bytes_read < packet_size)
    {
        printf("Packet data shortfall: expected %i, read %i \n", packet_size, bytes_read);
        return;
    }

    //loop over each packet segment
    //bytes_read =  recv(socket, receive_buffer, packet_size, 0 );

    if(bytes_read != packet_size) printf("TCP NETWORKING ARMAGEDDON!!! \n");

    process_packet_messages(receive_buffer, n1, packet_size, client_id);

    //printf("client_id= %i, packet_number= %i, packet_size= %i \n", client_id,packet_number,packet_size);
}

void NetPeer::flush_to_net() 
{
    if(connected == 0 )
    {
        printf("flush_outgoing_packets: Cannot send packet, disconnected!\n");
        return;
    }

    static int packet_number = 0;
    packet_number++;

    int n1 = 0;
    int packet_size = bn;

    PACK_uint16_t(client_id, buffer, &n1); //client id
    PACK_uint16_t(packet_number, buffer, &n1); //packet_number
    PACK_uint16_t(packet_size, buffer, &n1); //packet_number

    ENetPacket * packet = enet_packet_create(buffer, bn, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send (peer, 0, packet);
    //clear buffer

    enet_host_flush(enet_host);
    
    bn = 6;

}


int NetPeer::check_for_error()
{
    printf("NetPeer::check_for_error() \n");
    return 0;
}