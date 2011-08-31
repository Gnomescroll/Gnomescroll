#include "net_lib_common.h"
#include "server.h"

//uint32_t
//uint16_t

//ip in a.b.c.d format
struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port) {
    struct NetPeer s* = malloc(sizeof(struct NetPeer));

    unsigned int destination_address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;

    s->address.sin_family = AF_INET;
    s->address.sin_addr.s_addr = htonl( destination_address );
    s->address.sin_port = htons( destination_port );

    s->ip = htonl( destination_address )
    s->port = htons( destination_port )
    return s;
}

//pass in zero to get any port
struct Socket* create_socket(uint32_t IP, uint16_t  port) {
    //create socket
    struct Socket s* = malloc(sizeof(struct Socket));
    if(s==NULL) { printf("Malloc of socket failed.  Out of memory? \n"); return NULL;}

    s->handle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

    if ( s->handle <= 0 )
    {
        printf( "failed to create socket\n" );
        free(s);
        return NULL;
    }
    //bind socket

    //sockaddr_in address;
    s->address.sin_family = AF_INET;
    s->address.sin_addr.s_addr = INADDR_ANY;
    s->address.sin_port = htons( (unsigned short) port ); //set port 0 for any port

    s->port = port;
    //s->ip =  ?? do we know public IP?

    if ( bind( handle, (const sockaddr*) &s->address, sizeof(sockaddr_in) ) < 0 )
    {
        printf( "failed to bind socket\n" );
        free(s);
        return NULL;
    }
    printf("Socket bound to port %i\n", ntohl(s->port));

    //set socket to non-blocking

    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

        int nonBlocking = 1;
        if ( fcntl( s->handle, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
        {
            printf( "failed to set non-blocking socket\n" );
            free(s);
            return NULL;
        }

    #elif PLATFORM == PLATFORM_WINDOWS

        DWORD nonBlocking = 1;
        if ( ioctlsocket( s->handle, FIONBIO, &nonBlocking ) != 0 )
        {
            printf( "failed to set non-blocking socket\n" );
            free(s);
            return NULL;
        }

    #endif
    return s;
}

int send_packet(struct* Socket socket, char* packet_data, int packet_size) {

    int sent_bytes = sendto( s->handle, (const char*)packet_data, packet_size,
                             0, (sockaddr*)&socket_address, sizeof(sockaddr_in) );

    if ( sent_bytes != packet_size )
    {
        printf( "failed to send packet: return value = %d\n", sent_bytes );
        return false;
    }
}

#define maximum_packet_size 1024
void receive_packets(struct* Socket socket) {
    printf("Listening on port: %i\n", ntohl(s->port));

    unsigned char packet_data[maximum_packet_size];
    unsigned int from_address;
    unsigned int from_port;
    int received_bytes;

    while ( true )
    {


        #if PLATFORM == PLATFORM_WINDOWS
        typedef int socklen_t;
        #endif

        sockaddr_in from;
        socklen_t fromLength = sizeof( from );

        received_bytes = recvfrom( socket, (char*)packet_data, maximum_packet_size,
                                   0, (sockaddr*)&from, &fromLength );

        if ( received_bytes <= 0 )
            break;

        from_address = ntohl( from.sin_addr.s_addr );
        from_port = ntohs( from.sin_port );


        printf("received packet from: %i, %i\n", from_address, from_port);

        // process received packet
    }
}
