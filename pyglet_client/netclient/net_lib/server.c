#include "net_lib_common.h"
#include "server.h"

//uint32_t
//uint16_t

//ip in a.b.c.d format


//pass in zero to get any port
struct Socket* create_socket(uint32_t IP, uint16_t  port) {
    //create socket
    struct Socket* s = malloc(sizeof(struct Socket));
    if(s==NULL) { printf("Malloc of socket failed.  Out of memory? \n"); return NULL;}

    s->socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( s->socket <= 0 )
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

    if ( bind( s->socket, (const struct sockaddr*) &s->address, sizeof(struct sockaddr_in) ) < 0 )
    {
        printf( "failed to bind socket\n" );
        free(s);
        return NULL;
    }
    printf("Socket bound to port %i\n", port);

    //set socket to non-blocking

    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

        int nonBlocking = 1;
        if ( fcntl( s->socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
        {
            printf( "failed to set non-blocking socket\n" );
            free(s);
            return NULL;
        }

    #elif PLATFORM == PLATFORM_WINDOWS

        DWORD nonBlocking = 1;
        if ( ioctlsocket( s->socket, FIONBIO, &nonBlocking ) != 0 )
        {
            printf( "failed to set non-blocking socket\n" );
            free(s);
            return NULL;
        }

    #endif
    return s;
}

struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port) {
    struct NetPeer* s = malloc(sizeof(struct NetPeer));

    unsigned int destination_address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;

    s->address.sin_family = AF_INET;
    s->address.sin_addr.s_addr = htonl( destination_address );
    s->address.sin_port = htons( port );

    s->ip = htonl( destination_address );
    s->port = htons( port );
    return s;
}

int send_packet(struct Socket* socket, struct NetPeer* p, char* packet_data, int packet_size) {

    int sent_bytes = sendto( socket->socket, (const char*)packet_data, packet_size,
                             0, (const struct sockaddr*)&p->address, sizeof(struct sockaddr_in) );

    if ( sent_bytes != packet_size )
    {
        printf( "failed to send packet: return value = %d\n", sent_bytes );
        return 0;
    }
    return sent_bytes;
}

#define maximum_packet_size 1024
void receive_packets(struct Socket* s) {
    printf("Listening on port: %i\n", ntohl(s->port));

    unsigned char packet_data[maximum_packet_size];
    unsigned int from_address;
    unsigned int from_port;
    int received_bytes;

    while ( 1 )
    {


        #if PLATFORM == PLATFORM_WINDOWS
        typedef int socklen_t;
        #endif

        struct sockaddr_in from;
        socklen_t fromLength = sizeof( from );

        received_bytes = recvfrom( s->socket, (char*)packet_data, maximum_packet_size,
                                   0, (struct sockaddr*)&from, &fromLength );

        if ( received_bytes < 0 ) {

            if ( errno == EAGAIN ||  errno == EWOULDBLOCK) {
                continue;
            }
            if(received_bytes==-1) {
                perror("Socket Error:");
               printf("Network: socket error!\n");
               break;
            }
            perror("Socket Error:");
            printf("Network: negative received bytes, %i\n", received_bytes);
            break;
        }
        if ( received_bytes == 0) {
            printf("No new packets: continue\n");
            continue;
        }

        if(received_bytes > 0) {
            packet_data[received_bytes] = NULL;
            printf("packet: %s \n", packet_data);
        }

        from_address = ntohl( from.sin_addr.s_addr );
        from_port = ntohs( from.sin_port );


        printf("received packet from: %i, %i\n", from_address, from_port);

        // process received packet
    }
}
