#include "./net_peer.h"



//struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port) {
struct NetPeer* create_net_peer_by_remote_IP(int a, int b, int c, int d, unsigned short port) {
    struct NetPeer* s = (struct NetPeer*) malloc(sizeof(struct NetPeer));

    unsigned int destination_address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;

    s->client_id = 65535;
    s->connected = 0;

    s->address.sin_family = AF_INET;
    s->address.sin_addr.s_addr = htonl( destination_address );
    s->address.sin_port = htons( port );

    s->ip = htonl( destination_address );
    s->port = htons( port );

    s->ttl_max = TTL_MAX_DEFAULT;
    s->ttl = TTL_MAX_DEFAULT;
    s->last_packet_time = get_current_netpeer_time();

    init_sequencer(s);
///    init_sequence_numbers_out(&s->sq2); //init
///    init_sequence_numbers(&sq);
    return s;
}

//struct NetPeer* create_raw_net_peer(struct sockaddr_in address) {
struct NetPeer* create_net_peer_from_address(struct sockaddr_in address) {
    struct NetPeer* s = (struct NetPeer*) malloc(sizeof(struct NetPeer));
    s->client_id = 65535;
    s->address = address;

    s->ip = htonl(address.sin_addr.s_addr);
    s->port = htons( address.sin_port );

    s->ttl_max = TTL_MAX_DEFAULT;
    s->ttl = TTL_MAX_DEFAULT;
    s->last_packet_time = get_current_netpeer_time();
    //init_sequence_numbers_out(s); //init
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


int LAST_NETPEER_TIME = -1;
int CURRENT_NETPEER_TIME = -1;

void update_current_netpeer_time() {
    LAST_NETPEER_TIME = CURRENT_NETPEER_TIME;
    CURRENT_NETPEER_TIME = _GET_MS_TIME();
    return;
}

/*
int get_current_netpeer_time() {
    return CURRENT_NETPEER_TIME;
}
*/

void NP_print_delta() {
    printf("NP_print_delta: %i\n", CURRENT_NETPEER_TIME-LAST_NETPEER_TIME); //, CURRENT_NETPEER_TIME, LAST_NETPEER_TIME);
}

/*
//delta between time in past and current time
int NP_time_delta1(int time) {
    int delta = CURRENT_NETPEER_TIME - time;
    if( delta<0) {
        printf("NP_time_delta1: delta negative. rollover? \n");
        return 0;
    }
    return delta;
}

int NP_time_delta2(int t1, int t2) {
    int delta = t2 - t1;
    if(delta < 0) {
        delta = (16777216-t1) + t2
        printf("NP_time_delta2: delta negative. rollover? t1=%i,t2=%i,delta=%i \n", t1,t2,delta );
        return delta;
    }
    return delta;
}
//    delta = (int)(LAST_NETPEER_TIME-time)
//    if(abs((int)(time - LAST_NETPEER_TIME)))
*/
