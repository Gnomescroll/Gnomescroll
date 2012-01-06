#include "./net_peer.hpp"

//#include <sys/types.h>

#define NET_PEER_DEBUG 1

NetPeer::NetPeer() {}


//class NetMessageManager reliable_message_manager;
//class NetMessageManager unreliable_message_manager;
//class NetMessageManager python_message_manager;


void NetPeer::push_reliable_message(class Net_message* nm)
{
    
}

void NetPeer::push_python_message(class Net_message* nm)
{
    
}

void NetPeer::push_unreliable_packet(Net_message* nm) 
{
    
}


void NetPeer::flush_to_net() 
{
    if(this->connected == 0) 
    {
        printf("flush_outgoing_packets: Cannot send packet, disconnected!\n");
        return;
    }

    int n1 = 0;
    int seq = get_next_sequence_number(this);
    
    //printf("Sending Packet: seq= %i \n", seq);
    //pack header
    PACK_uint16_t(client_id, net_out_buff, &n1); //client id
    
    PACK_uint16_t(0, net_out_buff, &n1);  //packet_size
    PACK_uint16_t(seq, net_out_buff, &n1); //sequence number

    if(py_out.fcb.size != 0 && pending_reliable_bytes_out + pending_unreliable_bytes_out < 1450)
    {
        int max_bytes = 512;
        Net_message* nm = py_out.serialize_to_packet(max_bytes);
        push_reliable_packet(nm);
    }

    PACK_uint16_t(get_sequence_number(this), net_out_buff, &n1); //max seq
    PACK_uint32_t(generate_outgoing_ack_flag(this), net_out_buff, &n1); //sequence number


    //HEADER

    flush_unreliable_to_buffer(net_out_buff, &n1);
    flush_reliable_to_buffer(net_out_buff, &n1, &packet_sequence_buffer[seq%256]);

    #ifdef DC_CLIENT
    pviz_packet_sent(seq, n1);

    //simulate packet loss
    //static _i = 0;
    //if(_i<300 && seq % 2 == 0 ) return; //drop every 4th packet
    //_i++;

    int sent_bytes = sendto( NetClient::client_socket.socket, (const char*)net_out_buff, n1,0, (const struct sockaddr*)&this->address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != n1) { printf( "NetPeer::flush_to_net(): failed to send packet: return value = %i of %i\n", sent_bytes, n1 );}
    #endif

    #ifdef DC_SERVER
    //if(seq % 2 == 0 ) return; //drop every 4th packet
    int sent_bytes = sendto( NetServer::server_socket.socket, (const char*)net_out_buff, n1,0, (const struct sockaddr*)&this->address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != n1) { printf( "NetPeer::flush_to_net(): failed to send packet: return value = %i of %i\n", sent_bytes, n1 );}
    #endif

    if(sent_bytes < 0 ) printf("value less than zero\n"); //debug
    received_since_last_send = 0;
/*
	int len = 256;
	char error[256];
	error[0] = 0;
	#ifdef DC_SERVER
	int err = getsockopt(NetServer::server_socket.socket, SOL_SOCKET, SO_ERROR, error, &len);
	#else
	int err = getsockopt(NetClient::client_socket.socket, SOL_SOCKET, SO_ERROR, error, &len);
	#endif
	
	if(err != 0) printf("error: %i, \n", err);
	if(err != 0) printf("error: %i, %s \n", err, error);
*/
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
    //INVALID_SOCKET is -1
    if ( s->socket == -1 )
    { 
        #ifdef _WIN32
            int error_code = WSAGetLastError();
            printf( "Error: create socket failed, error code %i \n", error_code);
        #else
            //linux and mac
            printf( "Error: create socket failed\n");
        #endif
        free(s);
        return NULL;
    }
    if ( s->socket < 0 ) 
    {
        printf("Error: socket creation failed, socket is less than 0 and is not -1, is %i \n", s->socket);
        free(s);
        return NULL;
    }
    //bind socket
    s->address.sin_family = AF_INET;
    s->address.sin_addr.s_addr = INADDR_ANY;
    s->address.sin_port = htons( (unsigned short) port ); //set port 0 for any port

    if ( bind( s->socket, (const struct sockaddr*) &s->address, sizeof(struct sockaddr_in) ) < 0 ){printf( "failed to bind socket \n");free(s);return NULL;}
    printf("Socket bound to port %i\n", port);
    //set socket to non-blocking
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        int nonBlocking = 1;
        if ( fcntl( s->socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 ){printf( "failed to set non-blocking socket\n" );free(s);return NULL;}
    #elif PLATFORM == PLATFORM_WINDOWS
        DWORD nonBlocking = 1;
        if ( ioctlsocket( s->socket, FIONBIO, &nonBlocking ) != 0 ){printf( "failed to set non-blocking socket\n" );free(s);return NULL;}
    
		//int buffsize = 32768; // 65536
		int buffsize = 65536; // 65536
		setsockopt(s->socket, SOL_SOCKET, SO_RCVBUF, (void*)&buffsize, sizeof(buffsize));

	#endif
	
    return s;
}