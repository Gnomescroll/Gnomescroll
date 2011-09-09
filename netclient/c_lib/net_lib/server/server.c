
#include "server.h"


#define maximum_packet_size 1024
// SERVER SPECIFIC CONNECTION POOL STUF
struct ConnectionPool pool;
struct Socket server_socket;

unsigned int id_counter=0;


void init_server(unsigned short port) {
    update_current_netpeer_time();
    int i;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) pool.connection[i]=NULL;
    pool.n_connections = 0;

    struct Socket*s = create_socket(port);
    server_socket = *s;
    free(s);

    printf("Server Connection Pool Ready\n");
}

int pool_n_offset = 1;

int accept_connection(struct sockaddr_in from) {
    pool_n_offset++;
    struct NetPeer* p ;
    p = create_net_peer_from_address(from);

    int i,j;
    for(i=0;i<HARD_MAX_CONNECTIONS; i++) {
        j = (i+pool_n_offset)%HARD_MAX_CONNECTIONS;
        if(pool.connection[j]==NULL) {
            pool.connection[j] = p;
            p->client_id = j;
            p->ttl = p->ttl_max;
            p->last_packet_time = get_current_netpeer_time();
            p->connected = 0;
            //send client his id
            printf("Accepting Connection From: %i:%i \n",p->ip, p->port );
            return j;
        }
    }
    if(i >= HARD_MAX_CONNECTIONS) {printf("Could not open client connection: connection pool full!\n");free(p);return -1;}
    printf("Accept connection: error, should never reach this line!\n");
    return -1;
}

//min/max MTU is 576
//1500 is max MTU for ethernet

inline int error_check_packet(unsigned char* data, int n) {
    return 1;
}

void send_to_client(int client_id, unsigned char* buffer, int n) {
    //printf("Sending %i bytes to client %i \n", n, client_id);
    struct NetPeer* p;
    p = pool.connection[client_id];
    if(p == NULL) { printf("Send to client failed.  Client is null\n"); return; }

    //printf("Sending %i bytes to %i:%i\n",n, p->ip, p->port);
    int sent_bytes = sendto(server_socket.socket,(const char*)buffer, n,0, (const struct sockaddr*)&p->address, sizeof(struct sockaddr_in) );
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
            printf("New Client: sending client id\n");
            client_id = accept_connection(*from);
            send_id(client_id);
        }  else if ((client_id < HARD_MAX_CONNECTIONS) && (channel_id == 254) && (pool.connection[client_id] != NULL) ) {
            printf("Connection with client %i acked\n",  pool.connection[client_id]->client_id);
            pool.connection[client_id]->connected = 1;
            if( pool.connection[client_id]->client_id != client_id) printf("WTF ERROR: accept connection\n");
        }

        else {
            printf("validate_packet: Invalid 6 byte packet!\n");
        }
        return;
    }

    //crc check
    if(error_check_packet(buff,received_bytes) == 0) {printf("Packet failed CRC check!\n");return;}

    uint32_t value;
    int n1=0;

    UNPACK_uint16_t(&client_id, buff, &n1); //client id
    UNPACK_uint8_t(&channel_id, buff, &n1);  //channel 1

    UNPACK_uint16_t(&sequence_number, buff, &n1); //sequence number
    //ack string
    UNPACK_uint16_t(&max_seq, buff, &n1); //max seq
    UNPACK_uint32_t(&acks, buff, &n1); //sequence number

    //UNPACK_uint32_t(&value, buff, &n1);

    n=0;
    //UNPACK_uint16_t(&client_id, buff, &n);
    //UNPACK_uint8_t(&channel_id, buff, &n);
    //printf("Packet Received: cid= %i, seq= %i, bytes=%i \n", client_id, sequence_number, received_bytes);

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
    if(client_id != p->client_id) {
        printf("packet client_id is %i, expected %i\n", client_id, p->client_id);
        return;
    }

    set_ack_for_received_packet(p ,sequence_number);
    process_acks(p, max_seq, acks);
    p->ttl = p->ttl_max;
    p->last_packet_time = get_current_netpeer_time();

    //attempt to decode message in packet
    process_packet_messages(buff, n1, received_bytes);
    return;
}

unsigned char buffer[1500];

void process_packets() {

    struct sockaddr_in from;
    #if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
    #endif
    socklen_t fromLength = sizeof( from );

    int bytes_received;
    while(1) {
        bytes_received = recvfrom(server_socket.socket, buffer, 1500, 0, (struct sockaddr*)&from, &fromLength);
        if(bytes_received <= 0) return;
        process_packet(buffer, bytes_received, &from);
    }
}

void broad_cast_packet() {
    printf("broad_cast_packet deprecated:use broadcast 2\n");
}

void broad_cast_packet2(){

    int i,n1;
    struct NetPeer* p;
    int seq;

    unsigned char header[1500];

    for(i=0; i<HARD_MAX_CONNECTIONS; i++) {
        if(pool.connection[i] == NULL || pool.connection[i]->connected == 0) continue;
        p = pool.connection[i];
        if(p->connected == 0) { printf("Cannot send packet, disconnected: client %i\n",p->client_id); return;}

        n1 = 0;

        PACK_uint16_t(p->client_id, header, &n1); //client id
        PACK_uint8_t(1, header, &n1);  //channel 1

        //sequence number
        seq = get_next_sequence_number(p);
        PACK_uint16_t(seq, header, &n1); //sequence number

        //ack string
        PACK_uint16_t(get_sequence_number(p), header, &n1); //max seq
        PACK_uint32_t(generate_outgoing_ack_flag(p), header, &n1); //sequence number

        unsigned int value = 5;
        PACK_uint32_t(value, header, &n1);

        //if(seq % 5 == 0) return; //simulate packet loss
        send_to_client(i, header, n1);
        //printf("Sent packet %i to client %i\n", seq, p->client_id);
    }
}

void check_pool_for_dropped_packets() {
    int i;
    struct NetPeer* p;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) {
    if(pool.connection[i] == NULL) continue;
        p = pool.connection[i];
        check_for_dropped_packets(p);
    }
}

void poll_connection_timeout() {
    int i;
    struct NetPeer* p;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) {
    if(pool.connection[i] == NULL) continue;
        p = pool.connection[i];
        if(NP_time_delta1(p->last_packet_time) > 4000) { //4000 ms timeout
            printf("Connection %i timeout\n", p->client_id);
            pool.connection[i] = NULL;
            free(p);
        }
    }
}

void decrement_ttl() {
    printf("decrement ttl deprecated\n");
    int i;
    struct NetPeer* p;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) {
    if(pool.connection[i] == NULL) continue;
        p = pool.connection[i];
        p->ttl -= 1;
        if(p->ttl <= 0) {
            printf("Connection %i ttl expire: timeout\n", p->client_id);
            pool.connection[i] = NULL;
            free(p);
        }
    }

}

