
#include "server.h"
#include <net_lib/export.hpp>

namespace NetServer
{
//static const int maximum_packet_size 1024; // not used

// SERVER SPECIFIC CONNECTION POOL STUF

//struct ConnectionPool pool;
//struct Socket server_socket;

char buffer[1500]; //buffer for incoming packets


unsigned int id_counter=0;


void init_server(unsigned short port) {
    update_current_netpeer_time();
    int i;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) pool.connection[i]=NULL;
    pool.n_connections = 0;

    struct Socket*s = create_socket(port);
    server_socket = *s;
    free(s);

    printf("init server connection pool: port %i \n", (int) port);
}

int pool_n_offset = 0;

int accept_connection(struct sockaddr_in from) {
    pool_n_offset++;
    class NetPeer* p;
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

            //printf("Accepting Connection From: %i:%i \n",p->ip, p->port );
            printf("Accepting Connection From: %s:%i \n",inet_ntoa(p->address.sin_addr), p->port );

            client_connect_event(p->client_id);
            return j;
        }
    }
    if(i >= HARD_MAX_CONNECTIONS) {printf("Could not open client connection: connection pool full!\n"); delete p ;return -1;}
    printf("Accept connection: error, should never reach this line!\n");
    return -1;
}

//min/max MTU is 576
//1500 is max MTU for ethernet


void send_to_client(int client_id, char* buffer, int n)
{
    //printf("Sending %i bytes to client %i \n", n, client_id);
    class NetPeer* p;
    p = pool.connection[client_id];
    if(p == NULL) { printf("Send to client failed.  Client is null\n"); return; }

    //printf("Sending %i bytes to %i:%i\n",n, p->ip, p->port);
    //printf("client_id= %d.%d.%d.%d\n",p->address.sin_addr.S_un.S_un_b.s_b1,p->address.sin_addr.S_un.S_un_b.s_b2, p->address.sin_addr.S_un.S_un_b.s_b3,p->address.sin_addr.S_un.S_un_b.s_b4);
    //printf("address: %s\n", (char *)inet_ntoa((struct sockaddr_in)p->address.sin_addr));
    //printf("send_to_client: client ip address: %s\n", inet_ntoa(p->address.sin_addr));

    int sent_bytes = sendto(server_socket.socket,(const char*)buffer, n,0, (const struct sockaddr*)&p->address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != n)
    {
        printf( "send_to_client: failed to send packet: return value = %i, %i\n", sent_bytes, n );
        return;
    }
}


void send_id(uint16_t client_id) {
    printf("Sending Client Id\n");
    char buffer[6];
    int n=0;
    PACK_uint16_t(0, buffer, &n);
    PACK_uint16_t(client_id, buffer, &n);
    n=6;
    send_to_client(client_id, buffer, n);
}

uint16_t init_client(struct sockaddr_in* from) {
    printf("New Client: sending client id\n");
    uint16_t client_id = accept_connection(*from);
    send_id(client_id);
    send_id(client_id);
    send_id(client_id);
    ServerState::agent_list.send_to_client(client_id);
    return client_id;
}

void process_packet(char* buff, int received_bytes, struct sockaddr_in* from) {
    //printf("Packet\n");
    int n=0;

    uint16_t client_id;
    uint16_t packet_size;

    uint16_t sequence_number;
    uint16_t max_seq;
    uint32_t acks;

    class NetPeer* p;
    if(received_bytes < 6) { printf("Packet too small: %i bytes\n", received_bytes); return; }
    if(received_bytes == 6) {
        uint8_t channel_id;
        UNPACK_uint16_t(&client_id, buff, &n);
        UNPACK_uint8_t(&channel_id, buff, &n);

        if(client_id==65535 && channel_id == 255) {
            client_id = init_client(from);
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

    int n1=0;

    UNPACK_uint16_t(&client_id, buff, &n1); //client id
    UNPACK_uint16_t(&packet_size, buff, &n1);  //channel 1

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
        printf("Sender IP does not match client IP: %s, expected %s\n", inet_ntoa(from->sin_addr), inet_ntoa(p->address.sin_addr) );
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


    process_packet_messages(buff, n1, received_bytes, client_id);

    //ack stuff
    p->received_since_last_send++;
    if(p->received_since_last_send >= 8)
    {
        p->flush_to_net();
        printf("client.process_incoming_packets: client id= %i, force flush ack, more than 8 packets received between acks \n", p->client_id);
    }
    //attempt to decode message in packet
    return;
}

void process_packets() {

    struct sockaddr_in from;
    #if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
    #endif
    socklen_t fromLength = sizeof( from );

    int bytes_received;
    while(1) {
        bytes_received = recvfrom(server_socket.socket, (char*) buffer, 1500, 0, (struct sockaddr*)&from, &fromLength);
        if(bytes_received <= 0) return;
        process_packet(buffer, bytes_received, &from);
    }
}


void flush_packets()
{
    class NetPeer* np ;

    for(int i=0; i< HARD_MAX_CONNECTIONS; i++) 
    {
        np = pool.connection[i]; //use better iterator
        if(np == NULL) continue;
        if(np->connected == 0) { printf("Cannot send packet, disconnected: client %i\n",np->client_id); return;}
        np->flush_to_net();
    }

}

void check_pool_for_dropped_packets() {
    int i;
    class NetPeer* p;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) {
    if(pool.connection[i] == NULL) continue;
        p = pool.connection[i];
        check_for_dropped_packets(p);
    }
}

void poll_connection_timeout() {
    int i;
    class NetPeer* p;
    for(i=0; i<HARD_MAX_CONNECTIONS; i++) {
    if(pool.connection[i] == NULL) continue;
        p = pool.connection[i];

        int _time_since = NP_time_delta1(p->last_packet_time);

        //if( _time_since > 4000) { //4000 ms timeout
        if( _time_since > 10000) { //4000 ms timeout
            printf("Connection %i timeout\n", p->client_id);
            printf("Last packet at %i \n", _time_since);
            pool.connection[i] = NULL;
            client_disconnect_event(i);
            delete p; //need to clean up!
        }
    }
}

}
