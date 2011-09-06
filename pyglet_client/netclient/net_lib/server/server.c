
#include "server.h"


#define maximum_packet_size 1024
// SERVER SPECIFIC CONNECTION POOL STUF
struct ConnectionPool pool;
unsigned int id_counter=0;


void init_server(unsigned short port) {
    int i;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) pool.connection[i]=NULL;
    pool.n_connections = 0;

    struct Socket* s = create_socket(0, port);
    if(s==NULL) {
        printf("Socket is NULL: init_server failed\n"); return;
    }
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
            printf("Accepting Connection From: %i:%i \n",p->ip, p->port );
            return i;
        }
    }
    if(p->id == 0) {
        printf("Could not open client connection: connection pool full!\n");
        free(p);
        return -1;
    }
    printf("Accept connection: error, should never reach this line!\n");
    return -2;
    //send client client id
}

//min/max MTU is 576
//1500 is max MTU for ethernet

inline int error_check_packet(unsigned char* data, int n) {
    return 1;
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

/*
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

        received_bytes = recvfrom( s->socket, (char*)packet_data, maximum_packet_size, 0, (struct sockaddr*)&from, &fromLength );

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
*/

void send_to_client(int client_id, unsigned char* buffer, int n) {
    struct NetPeer* p;
    p = pool.connection[client_id];
    if(p == NULL) { printf("Send to client failed.  Client is null\n"); return; }

    printf("Sending %i bytes to %i:%i\n",n, p->ip, p->port);
    int sent_bytes = sendto(pool.socket.socket,(const char*)buffer, n,0, (const struct sockaddr*)&p->address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != n)
    {
        printf( "send_to_client: failed to send packet: return value = %i, %i\n", sent_bytes, n );
        return;
    }
}


void send_id(uint16_t client_id) {
    printf("Sending Client Id\n");
    unsigned char buffer[6];
    int n=0;
    PACK_uint16_t(0, buffer, &n);
    PACK_uint16_t(client_id, buffer, &n);
    n=6;
    send_to_client(client_id, buffer, n);
}

void process_packet(unsigned char* buff, int received_bytes, struct sockaddr_in* from) {
    //printf("Packet\n");
    int n=0;

    uint16_t client_id;
    uint8_t channel_id;

    uint16_t sequence_number;
    uint16_t max_seq;
    uint32_t acks;

    struct NetPeer* p;
    if(received_bytes < 6) { printf("Packet too small: %i bytes\n", received_bytes); return; }
    if(received_bytes == 6) {

        UNPACK_uint16_t(&client_id, buff, &n);
        UNPACK_uint8_t(&channel_id, buff, &n);

        if(client_id==65535 && channel_id == 255) {
            client_id = accept_connection(*from);
            send_id(client_id);
        }  else {
            printf("validate_packet: Invalid 6 byte packet!\n");
        }
        return;
    }


    //crc check
    if(error_check_packet(buff,received_bytes) == 0) {
        printf("Packet failed CRC check!\n");
        return;
    }

    //uint8_t channel_id;
    //uint16_t client_id;

    uint32_t value;

    int n1=0;

    UNPACK_uint16_t(&client_id, buff, &n1); //client id
    UNPACK_uint8_t(&channel_id, buff, &n1);  //channel 1

    UNPACK_uint16_t(&sequence_number, buff, &n1); //sequence number
    //ack string
    UNPACK_uint16_t(&max_seq, buff, &n1); //max seq
    UNPACK_uint32_t(&acks, buff, &n1); //sequence number

    UNPACK_uint32_t(&value, buff, &n1);

    n=0;
    //UNPACK_uint16_t(&client_id, buff, &n);
    //UNPACK_uint8_t(&channel_id, buff, &n);
    printf("Packet Received: cid= %i, seq= %i, bytes=%i \n", client_id, sequence_number, received_bytes);

    if(client_id >= HARD_MAX_CONNECTIONS) {
        printf("Client id %i exceeds HARD_MAX_CONNECTIONS\n", client_id);
        return;
    }
    if(pool.connection[client_id] != NULL) {
        p = pool.connection[client_id];
    } else {
        printf("Client id NetClient is NULL\n");
        return;
    }
    if(from->sin_addr.s_addr != p->address.sin_addr.s_addr) {
        printf("Sender IP does not match client IP: %i, expected %i\n", htonl(from->sin_addr.s_addr), htonl(p->address.sin_addr.s_addr) );
        return;
    }
    if(client_id != p->id) {
        printf("packet client_id is %i, expected %i\n", client_id, p->id);
        return;
    }

    set_ack_for_received_packet(&p->sq2 ,sequence_number);
    p->ttl = TTL_MAX;
    //
    //process_sequence_number(p, sequence_number);
    //

    //printf("Packet received from client %i\n", p->id);
    //buff[received_bytes] = 0;
    //printf("Packet= %s \n", buff);
    return;


}

unsigned char buffer[1500];

struct timeval timeout;
fd_set read_flags;
fd_set write_flags;

void process_packets() {
    //int n=0;
    //int received_bytes;

    struct sockaddr_in from;
    //timeout.tv_sec = 0;
    //timeout.tv_usec = 0;

    #if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
    #endif

    socklen_t fromLength = sizeof( from );


    int bytes_received;
    while(1) {

        //sockaddr_in from;

        bytes_received = recvfrom(pool.socket.socket, buffer, 1500, 0, (struct sockaddr*)&from, &fromLength);

        if(bytes_received <= 0) return;
        process_packet(buffer, bytes_received, &from);
    }
}

int SEQ = 500; //hack for sequence number

void broad_cast_packet() {
    SEQ +=1;

    int i,n1;
    struct NetPeer* p;
    unsigned char header[1500];

    for(i=0; i<HARD_MAX_CONNECTIONS; i++) {
    if(pool.connection[i] == NULL) continue;
        p = pool.connection[i];
        n1=0;

        int seq = 0;// get_next_sequence_number(&sq);
        seq = SEQ;

        PACK_uint16_t(0, header, &n1); //server id
        PACK_uint8_t(1, header, &n1);  //channel 1
        PACK_uint16_t(seq, header, &n1); //sequence number

        //ack string
        PACK_uint16_t(get_sequence_number(&p->sq2), header, &n1); //max seq
        PACK_uint32_t(generate_outgoing_ack_flag(&p->sq2), header, &n1); //sequence number

        unsigned int value = 5;
        PACK_uint32_t(value, header, &n1);


        send_to_client(i, header, n1);
    }
}

void decrement_ttl() {
    int i;
    struct NetPeer* p;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) {
    if(pool.connection[i] == NULL) continue;
        p = pool.connection[i];
        p->ttl -= 1;
        if(p->ttl <= 0) {
            printf("Connection %i ttl expire: timeout\n", p->id);
            pool.connection[i] = NULL;
            free(p);
        }
    }

}

