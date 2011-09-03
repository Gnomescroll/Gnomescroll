
#include "server.h"

//uint32_t
//uint16_t

//ip in a.b.c.d format


//pass in zero to get any port


#define maximum_packet_size 1024
// SERVER SPECIFIC CONNECTION POOL STUF
struct ConnectionPool pool;
unsigned int id_counter=0;

struct Socket* create_socket(uint32_t IP, uint16_t port) {
    //create socket
    struct Socket* s = (struct Socket*) malloc(sizeof(struct Socket));
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
    struct NetPeer* s = (struct NetPeer*) malloc(sizeof(struct NetPeer));
    s->id = 0;

    unsigned int destination_address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;

    s->address.sin_family = AF_INET;
    s->address.sin_addr.s_addr = htonl( destination_address );
    s->address.sin_port = htons( port );

    s->ip = htonl( destination_address );
    s->port = htons( port );
    return s;
}

struct NetPeer* create_raw_net_peer(struct sockaddr_in address) {
    struct NetPeer* s = (struct NetPeer*) malloc(sizeof(struct NetPeer));
    s->id = 0;
    s->address = address;

    s->ip = address.sin_addr.s_addr;
    s->port = htons( address.sin_port );
    return s;
}

int send_packet(struct Socket* socket, struct NetPeer* p, unsigned char* packet_data, int packet_size) {

    int sent_bytes = sendto(socket->socket,(const char*)packet_data, packet_size,0, (const struct sockaddr*)&p->address, sizeof(struct sockaddr_in) );

    if ( sent_bytes != packet_size )
    {
        printf( "failed to send packet: return value = %d\n", sent_bytes );
        return 0;
    }
    return sent_bytes;
}

void receive_packets(struct Socket* s) {
    printf("Listening on port: %i\n", ntohl(s->port));

    unsigned char packet_data[maximum_packet_size];
    unsigned int from_address;
    unsigned int from_port;
    //unsigned int to_port;
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
            //if packet... then  accept_connection(from)
            packet_data[received_bytes] = 0;
            printf("packet: %s \n", packet_data);
        }

        from_address = ntohl( from.sin_addr.s_addr );
        from_port = ntohs( from.sin_port );
        //to_port = ntohs( from.sout_port );

        printf("received packet from: %i, %i\n", from_address, from_port);

        // process received packet
    }
}


void init_server(unsigned short port) {
    int i;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) pool.connection[i]=NULL;
    pool.n_connections = 0;

    struct Socket* s = create_socket(0, port);
    pool.socket =  *s;
    if(s != NULL) free(s);

    printf("Server Connection Pool Ready\n");
}

int accept_connection(struct sockaddr_in from) {

    struct NetPeer* p ;
    p = create_raw_net_peer(from);

    int i;
    for(i=0;i<HARD_MAX_CONNECTIONS; i++) {
        if(pool.connection[i]==NULL) {
            pool.connection[i] = p;
            p->id = i;
            p->ttl = 100;
            //send client his id
            printf("Accepting Connection From: %i:%i \n",htonl(p->ip), p->port );
            return i;
        }
    }
    if(p->id == 0) {
        printf("Could not open client connection: connection pool full!\n");
        return;
    }

    //send client client id
}

//min/max MTU is 576
//1500 is max MTU for ethernet

inline int error_check_packet(unsigned char* data, int n) {
    return 1;
}

void send_to_client(int client_id, unsigned char* buffer, int n) {
    struct NetPeer* p;
    p = pool.connection[client_id];
    if(p == NULL) { printf("Send to client failed.  Client is null\n"); return; }
    int sent_bytes = sendto(socket->socket,(const char*)buffer, n,0, (const struct sockaddr*)&p->address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != packet_size )
    {
        printf( "send_to_client: failed to send packet: return value = %d\n", sent_bytes );
        return 0;
    }


}

void send_id(uint16_t client_id) {
    unsigned char buffer[6];
    int n=0;
    PACK_uint16_t(0, buffer, &n);
    PACK_uint16_t(client_id, buffer, &n);
    n=6;
    send_to_client(client_id, buffer, &n)
}

fd_set read_flags,write_flags

void process_packets() {

    unsigned char packet_data[maximum_packet_size];
    //unsigned int from_address;
    //unsigned int from_port;
    //unsigned int to_port;
    int received_bytes;

    uint16_t client_id;
    uint8_t channel_id;
    //int i;
    //use select or epoll!!
    //for(i=0;i<HARD_MAX_CONNECTIONS; i++) {

    struct NetPeer* p;

    #if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
    #endif

    struct sockaddr_in from;
    socklen_t fromLength = sizeof( from );

    //if(!select(1, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout);

    if(0) {
        ///hack
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(pool.socket.socket, &fds);
        int rc;

       struct timeval timeout;
       timeout.tv_sec = 20;
       timeout.tv_usec = 0;

        rc = select(sizeof(fds)*8, &fds, NULL, NULL, &timeout);
        if (rc==-1) {
            perror("select failed");
            return;// -1;
        }
        printf("rc= %i \n", rc);

        if (FD_ISSET(pool.socket.socket, &fds)) {
             printf("Packet ready!\n");
        } else {
            //printf("Packet ready!\n");
            return;
        }
    }
    ///hack
    received_bytes = recvfrom( pool.socket.socket, (char*)packet_data, maximum_packet_size, 0, (struct sockaddr*)&from, &fromLength );

    if(received_bytes == 6) {
        n=0;
        UNPACK_uint16_t(&client_id, buffer, &n);
        UNPACK_uint8_t(&channel_id, buffer, &n);

        if(client_id==0 && message_id == 255) {
            client_id = accept_connection(from);
            send_id(client_id,from);
        }  else {
            printf("Invalid 6 byte packet!\n");
        }
    }
    else if(received_bytes > 6) {
        //crc check
        if(error_check_packet(packet_data,received_bytes) == 0) {
            printf("Packet failed CRC check!\n");
            return;
        }

        n=0;
        UNPACK_uint16_t(&client_id, buffer, &n)
        UNPACK_uint8_t(&channel_id, buffer, &n)
        printf("client id= %i\n", client_id);


        p = pool.connection[client_id];
        if(client_id >= HARD_MAX_CONNECTIONS || p==NULL || from.sin_addr.s_addr != p->ip || id != p->id) {
            printf("Received packet from connection with invalid IP, client_id pair\n");
            return;
        }
        printf("Packet received from client %i\n", p->id);
        packet_data[received_bytes] = 0;
        printf("Packet= %s \n", packet_data);
        return;
        }
    }
}

void decrement_ttl() {
    int i;
    struct NetPeer* p;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) {
    if(pool.connection[i] == NULL) continue;
        p = pool.connection[i];
        p.ttl -= 1;
        if(p.ttl <= 0) {
            printf("Connection %i ttl expire!\n", p.id);
            pool.connection[i] = NULL;
            free(p);
        }
    }

}

