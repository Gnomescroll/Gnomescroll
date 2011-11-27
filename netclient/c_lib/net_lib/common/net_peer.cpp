#include "./net_peer.hpp"

#include <net_lib/client/client.hpp>

static char net_out_buff[2000];

/*
    Use arrays/pointers/pool later for packets, to remove limits
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
    rnma_insert_index++;
    if(rnma_insert_index == NET_MESSAGE_ARRAY_SIZE)
    {
        rnma_insert_index = 0;
        rnma_insert->next = NetMessageArray::acquire();
        rnma_insert = rnma_insert->next;
    }
}

/*
void NetPeer::push_reliable_packet(Net_message* np) {
    reliable_net_message_array[reliable_net_message_array_index] = np;
    reliable_net_message_array_index++;
    pending_bytes_out += np->len;
    //nm->reference_count++;
    if(reliable_net_message_array_index > 256) printf("Net_message_list Push_reliable_packet overflow 2\n");     //debug
}
*/
 
//void * memcpy ( void * destination, const void * source, size_t num );
void NetPeer::flush_unreliable_to_buffer(char* buff_, int* _index) {
    //if(pending_bytes_out > 1500) printf("NetPeer Error 1: too much data in packet buffer, %i \n", pending_bytes_out);
    if(pending_unreliable_bytes_out > 1500) printf("NetPeer Error 2: unreliable bytes out exceeds 1500, %i \n", pending_unreliable_bytes_out)
    Net_message* nm;
    int index = *_index;
    for(int i=0; i< unreliable_net_message_array_index; i++)
    {
        nm = unreliable_net_message_array[i];
        memcpy(buff_+index, nm->buff, nm->len);
        index += nm->len;
        nm->decrement_unreliable();
    }

    
    //reliable packets
    /*
    for(int i=0; i< reliable_net_message_array_index; i++)
    {
        nm = unreliable_net_message_array[i];
        memcpy(buff_+index, nm->buff, nm->len);
        index += nm->len;
    }
    */

    pending_bytes_out = 0;
    unreliable_net_message_array_index = 0;

    *_index = index;    //bytes out
}


void NetPeer::flush_reliable_to_buffer(char* buff_, int* _index, struct* packet_sequence)
{
    //see if there is room for channel bytes
    /*
    if(channel_out_byte != 0)
    {
        int free_bytes = 1500 - (pending_reliable_bytes_out + index)
        while(free_bytes > 256) //pad to 1500 with channel packets
    }
    if(pending_reliable_bytes_out + index < 1500)
    { 
        pop stuff from channel onto stack
    }
    else
    {
           
    }
    */

    int index = *_index;
    class Net_message* nm;
    const NetMessageArray* nma = rnma_read;
    const int read_index = rnma_read_index;
    const int num = rnma_pending_messages;

    /*
        Retire on packet ack, not on dispatch
    */
    for(int i=0; i < num; i++)
    {
        nm = nma->net_message_array[read_index];
        //do something

        nm = unreliable_net_message_array[i];
        memcpy(buff_+index, nm->buff, nm->len);
        index += nm->len;

        nma->reference_count--;

        read_index++;
        if(read_index == NET_MESSAGE_ARRAY_SIZE)
        {
            //if(nma->reference_count == 0) nma->retire(); //check 1
            nma = nma->next;
            read_index=0;
        }
    }
    //if(nma->reference_count == 0) nma->retire(); //check 2

    //channel send here
    /*
        Channels write to buffer
        Channels use the reliable_net_message delivery
        reliable net_messages encapsolate buffer
    */


    rnma_pending_messages = 0;
    reliable_net_message_array_index = 0;

    *_index = index;    //bytes out
}

void NetPeer::flush_to_net() {
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

    
    //printf("n_bytes out= %i \n", n1);

    #ifdef DC_CLIENT
    pviz_packet_sent(seq, n1);
    //int sent_bytes = sendto( client_socket.socket, (const char*)client_out_buff, client_out_buff_n,0, (const struct sockaddr*)&NPserver.address, sizeof(struct sockaddr_in) );
    int sent_bytes = sendto( NetClient::client_socket.socket, (const char*)net_out_buff, n1,0, (const struct sockaddr*)&this->address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != n1) { printf( "NetPeer::flush_to_net(): failed to send packet: return value = %i of %i\n", sent_bytes, n1 );}
    #endif

}


void reset_NetPeer_buffer(class NetPeer* s) {
    s->buff_n = 11; //size of header
}

//class NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port) {
class NetPeer* create_net_peer_by_remote_IP(int a, int b, int c, int d, unsigned short port) {
    //class NetPeer* s = (class NetPeer*) malloc(sizeof(class NetPeer));
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

    reset_NetPeer_buffer(s);
    init_sequencer(s);
///    init_sequence_numbers_out(&s->sq2); //init
///    init_sequence_numbers(&sq);
    return s;
}

//class NetPeer* create_raw_net_peer(struct sockaddr_in address) {
class NetPeer* create_net_peer_from_address(struct sockaddr_in address) {
    //class NetPeer* s = (class NetPeer*) malloc(sizeof(class NetPeer));
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
    //init_sequence_numbers_out(s); //init
    reset_NetPeer_buffer(s);
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