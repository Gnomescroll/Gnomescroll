#include "./net_peer.hpp"

#include <net_lib/client/client.hpp>
#include <net_lib/server/server.h>

static char net_out_buff[2000];

/*
    Use arrays/pointers/pool later for packets, to remove limits
*/

/*
Possible Memory leak
*/

void NetPeer::push_unreliable_packet(Net_message* nm) 
{
    pending_unreliable_bytes_out += nm->len;
    pending_bytes_out += nm->len;

    nm->reference_count++;
    unreliable_net_message_array[unreliable_net_message_array_index] = nm;
    unreliable_net_message_array_index++;
    if(unreliable_net_message_array_index > 256) printf("Net_message_list Push_unreliable_packet overflow 1\n");   //debug
    //would require all messages were less than 6 bytes to fit 256 messagse into 1500 byte packet
}

void NetPeer::push_reliable_packet(class Net_message* nm)
{
    pending_bytes_out += nm->len; //
    pending_reliable_bytes_out += nm->len;
    rnma_pending_messages++;

    nm->reference_count++;  //increment reference count?
    pending_reliable_packets_out++;

    rnma_insert->net_message_array[rnma_insert_index] = nm;
    rnma_insert->reference_count++;
    printf("packet pushed: ref count = %i\n", rnma_insert->reference_count);
    rnma_insert_index++;
    if(rnma_insert_index == NET_MESSAGE_ARRAY_SIZE)
    {
        //DEBUG?
        rnma_insert->reference_count--;
        NetMessageArray* tmp = rnma_insert->next;
        rnma_insert = NetMessageArray::acquire();
        tmp->next = rnma_insert;
        if(tmp->reference_count == 0) tmp->retire();
        rnma_insert_index = 0;
        rnma_insert->reference_count = 1;

    }
}

//void * memcpy ( void * destination, const void * source, size_t num );
void NetPeer::flush_unreliable_to_buffer(char* buff_, int* _index) {
    //if(pending_bytes_out > 1500) printf("NetPeer Error 1: too much data in packet buffer, %i \n", pending_bytes_out);
    if(pending_unreliable_bytes_out > 1500) printf("NetPeer Error 2: unreliable bytes out exceeds 1500, %i \n", pending_unreliable_bytes_out);
    class Net_message* nm;
    int index = *_index;
    for(int i=0; i< unreliable_net_message_array_index; i++)
    {
        nm = unreliable_net_message_array[i];
        memcpy(buff_+index, nm->buff, nm->len);
        index += nm->len;
        nm->decrement_unreliable();
    }

    pending_bytes_out = 0;
    pending_unreliable_bytes_out = 0;

    unreliable_net_message_array_index = 0;

    *_index = index;    //bytes out
}


void NetPeer::flush_reliable_to_buffer(char* buff_, int* _index, struct packet_sequence* ps)
{

    ps->nma = rnma_read;
    ps->read_index = rnma_read_index;
    ps->messages_n = rnma_pending_messages;

    if(rnma_pending_messages == 0) return;

    int index = *_index;
    if(pending_reliable_bytes_out + index > 1500) printf("NetPeer error: reliable bytes out overflows 1500 byte buffer\n");

    class Net_message* nm;

    /*
        Retire on packet ack, not on dispatch
    */

    for(int i=0; i < rnma_pending_messages; i++)
    {
        nm = rnma_read->net_message_array[rnma_read_index];
        //do something
        memcpy(buff_+index, nm->buff, nm->len);
        index += nm->len;
        //rnma_nma->reference_count--; //wtf, decrement on confirmation
        rnma_read_index++;
        if(rnma_read_index == NET_MESSAGE_ARRAY_SIZE)
        {
            rnma_read = rnma_read->next;
            rnma_read_index=0;
        }
    }

    //channel send here
    /*
        Channels write to buffer
        Channels use the reliable_net_message delivery
        reliable net_messages encapsolate buffer
    */
    pending_reliable_bytes_out = 0;
    rnma_pending_messages = 0;

    *_index = index;    //bytes out
}

void NetPeer::resend_packet(struct packet_sequence* ps)
{
    if(ps->messages_n == 0) return;

    NetMessageArray* nma = ps->nma;
    int nma_index = ps->read_index;
    int num = ps->messages_n;

    class Net_message* nm;

    for(int i=0; i < num; i++)
    {
        nm = nma->net_message_array[nma_index];
        push_reliable_packet(nm);
        nm->reference_count--; //will increment again when pushed

        nma_index++;
        if(nma_index == NET_MESSAGE_ARRAY_SIZE)
        {
            nma = nma->next;
            nma_index=0;
        }
    }

}

void NetPeer::ack_packet(struct packet_sequence* ps)
{
    if(ps->messages_n == 0) return;

    NetMessageArray* nma = ps->nma;
    int nma_index = ps->read_index;
    int num = ps->messages_n;

    class Net_message* nm;

    //for(int i=0; i < num; i++)
    /*        
        Iterate over num packets
        Dont let retire check 1 on last loop
    */
    for(int i=0; i < num; i++)
    {
        nm = nma->net_message_array[nma_index];
        nma->net_message_array[nma_index] = NULL; //!!! DEBUG

        nm->decrement_reliable();
        nma->reference_count--;    //decrement on confirmation
        nma_index++;
        
        if(nma_index == NET_MESSAGE_ARRAY_SIZE)
        {
            NetMessageArray* tmp = nma->next;
            if(nma->reference_count == 0)
            {
                if(i+1 == num) break;   //prevent from running on last loop, to avoid double retiring
                //printf("1 delete nma %i \n", (int)nma);
                nma->retire();
            }
            nma = tmp;
            nma_index=0;
        }
    }
    if(nma->reference_count == 0)
    {
        //printf("2 delete nma %i \n", (int) nma);
        nma->retire();
    } 
}

void NetPeer::flush_to_net() 
{
    int n1 = 0;
    int seq = get_next_sequence_number(this);
    
    //pack header
    PACK_uint16_t(client_id, net_out_buff, &n1); //client id
    PACK_uint8_t(1, net_out_buff, &n1);  //channel 1
    PACK_uint16_t(seq, net_out_buff, &n1); //sequence number
    PACK_uint16_t(get_sequence_number(this), net_out_buff, &n1); //max seq
    PACK_uint32_t(generate_outgoing_ack_flag(this), net_out_buff, &n1); //sequence number

    flush_unreliable_to_buffer(net_out_buff, &n1);
    flush_reliable_to_buffer(net_out_buff, &n1, &packet_sequence_buffer[seq%256]);

    if(this->connected == 0) 
    {
        printf("flush_outgoing_packets: Cannot send packet, disconnected!\n");
        return;
    }

    
    
    #ifdef DC_CLIENT
    pviz_packet_sent(seq, n1);

    //simulate packet loss
    //if(seq % 2 == 0 ) return; //drop every 4th packet

    int sent_bytes = sendto( NetClient::client_socket.socket, (const char*)net_out_buff, n1,0, (const struct sockaddr*)&this->address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != n1) { printf( "NetPeer::flush_to_net(): failed to send packet: return value = %i of %i\n", sent_bytes, n1 );}
    #endif

    #ifdef DC_SERVER
    //if(seq % 2 == 0 ) return; //drop every 4th packet
    int sent_bytes = sendto( NetServer::server_socket.socket, (const char*)net_out_buff, n1,0, (const struct sockaddr*)&this->address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != n1) { printf( "NetPeer::flush_to_net(): failed to send packet: return value = %i of %i\n", sent_bytes, n1 );}
    #endif
}

class NetPeer* create_net_peer_by_remote_IP(int a, int b, int c, int d, unsigned short port) {
    class NetPeer* s = new NetPeer;
    
    unsigned int destination_address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;

    s->client_id = 65535;
    s->connected = 0;

    s->address.sin_family = AF_INET;
    s->address.sin_addr.s_addr = htonl( destination_address );
    s->address.sin_port = htons( port );

    s->ip = ntohl( destination_address );
    s->port = ntohs( port );

    s->ttl_max = TTL_MAX_DEFAULT;
    s->ttl = TTL_MAX_DEFAULT;
    s->last_packet_time = get_current_netpeer_time();

    init_sequencer(s);

    return s;
}

class NetPeer* create_net_peer_from_address(struct sockaddr_in address) {
    class NetPeer* s = new NetPeer;
    s->client_id = 65535;
    s->address = address;

    //should be excessive/uneeded
    s->address.sin_family = AF_INET;
    s->address.sin_addr.s_addr = address.sin_addr.s_addr;
    s->address.sin_port = address.sin_port;

    s->ip = ntohl(address.sin_addr.s_addr);
    s->port = ntohs( address.sin_port );

    printf("create_net_peer_from_address: %s:%i \n",inet_ntoa(s->address.sin_addr), ntohs( address.sin_port ) );

    s->ttl_max = TTL_MAX_DEFAULT;
    s->ttl = TTL_MAX_DEFAULT;
    s->last_packet_time = get_current_netpeer_time();

    init_sequencer(s);
    return s;
}

struct Socket* create_socket(uint16_t port) {
    //create socket
    struct Socket* s = (struct Socket*) malloc(sizeof(struct Socket));
    s->ip=0;
    s->port = port;
    if(s==NULL) { printf("Malloc of socket failed.  Out of memory? \n"); return NULL;}
    s->socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( s->socket <= 0 ){ printf( "failed to create socket\n" );free(s);return NULL;}
    //bind socket
    s->address.sin_family = AF_INET;
    s->address.sin_addr.s_addr = INADDR_ANY;
    s->address.sin_port = htons( (unsigned short) port ); //set port 0 for any port

    if ( bind( s->socket, (const struct sockaddr*) &s->address, sizeof(struct sockaddr_in) ) < 0 ){printf( "failed to bind socket\n" );free(s);return NULL;}
    printf("Socket bound to port %i\n", port);
    //set socket to non-blocking
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        int nonBlocking = 1;
        if ( fcntl( s->socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 ){printf( "failed to set non-blocking socket\n" );free(s);return NULL;}
    #elif PLATFORM == PLATFORM_WINDOWS
        DWORD nonBlocking = 1;
        if ( ioctlsocket( s->socket, FIONBIO, &nonBlocking ) != 0 ){printf( "failed to set non-blocking socket\n" );free(s);return NULL;}
    #endif
    return s;
}