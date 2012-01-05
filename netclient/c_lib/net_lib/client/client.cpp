#include "./client.hpp"
#include <string.h>

#include "../common/sequencer.h"

#include <net_lib/export.hpp>

namespace NetClient {

class NetPeer NPserver;
struct Socket client_socket;

//class NetPeer NPserver;
//struct Socket client_socket;

//used for incoming data
char buffer[4096]; //1500 is max ethernet MTU


/*
    Port needs to be passed in
*/
void init_client() {

    NPserver.init();
    update_current_netpeer_time();
    init_sequencer(&NPserver);
    

    int local_port = 6967+(rand()%32); ///should randomize!
    //int local_port =  8989;

    struct Socket* s = create_socket(local_port);

    if(s != NULL) delete s ;
    client_socket = *s;

    init_sequencer(&NPserver);
}

class NetPeer* CLIENT_get_NP() {
    return &NPserver;
}

void set_server(int a, int b, int c, int d, unsigned short port) {
    NPserver.ip =  ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
    NPserver.port = port;

    class NetPeer* p = create_net_peer_by_remote_IP(a,b,c,d,port);
    NPserver = *p;
    //free(p);
    delete p;
}

void attempt_connection_with_server() {
    if(NPserver.connected == 1) {
        printf("attempt_connection_with_server: Error! client is already connected\n");
    }
    init_sequencer(&NPserver); //virgin state on reconnect
    char buff[6];
    int n=0;
    PACK_uint16_t(65535, buff, &n);
    PACK_uint8_t(255, buff, &n);
    n=6; //must be 6 bytes
    int sent_bytes = sendto( client_socket.socket, (const char*)buff, n,0, (const struct sockaddr*)&NPserver.address, sizeof(struct sockaddr_in) );
    if ( sent_bytes <= 0) { printf( "attempt_connection_with_server: failed to send packet: return value = %i\n", sent_bytes); return;}
    else { printf("Client id requested\n");}
}

void ack_connection_with_server() {
    char buff[6];
    int n=0;
    PACK_uint16_t(NPserver.client_id, buff, &n);
    PACK_uint8_t(254, buff, &n);
    n=6; //must be 6 bytes
    int sent_bytes = sendto( client_socket.socket, (const char*)buff, n,0, (const struct sockaddr*)&NPserver.address, sizeof(struct sockaddr_in) );
    //printf("ack_connection_with_server\n");
    if ( sent_bytes <= 0) { printf( "ack_connection_with_server: failed to send packet: return value = %i\n", sent_bytes); return;}
}

int validate_packet(char* buff, int n, struct sockaddr_in* from) {
    if(n <= 0) return 1;
    if(NPserver.connected == 0 && n ==6) { //server connection packet
        int n1=0;
        uint16_t client_id;
        uint16_t channel_id;
        UNPACK_uint16_t(&channel_id, buff, &n1);
        UNPACK_uint16_t(&client_id, buff, &n1);
        printf("Received client id= %i\n",client_id);
        NPserver.client_id = client_id;
        NPserver.connected = 1;
        NPserver.last_packet_time = get_current_netpeer_time();
        //server.server_address.sin_addr.s_addr = from->sin_addr.s_addr;
        //server.server_address.sin_port = from->sin_port;
        printf("Client id assigned: %i server: %i:%i\n", client_id, htonl(from->sin_addr.s_addr), ntohs( from->sin_port ));
        ack_connection_with_server();

        client_connect_event(NPserver.client_id);
        return 0;
    }

    if(from->sin_port != NPserver.address.sin_port) {
        unsigned short from_port = ntohs( from->sin_port );
        printf("rogue %i byte packet from IP= %s:%i, expected Server IP = %s:%i\n", n, inet_ntoa(from->sin_addr),from_port, inet_ntoa(NPserver.address.sin_addr), ntohs(NPserver.address.sin_port));

        return 1;
    }
    return 1;


    return 0; //does not validate
}

#if PLATFORM == PLATFORM_WINDOWS
typedef int socklen_t;
#endif
    
void process_incoming_packets() {
    struct sockaddr_in from;
    socklen_t fromLength = sizeof( from );
    int bytes_received;

    while(1) {
        bytes_received = recvfrom(client_socket.socket, (char*)buffer, 4096, 0, (struct sockaddr*)&from, &fromLength);
        if(bytes_received <= 0) {return;}
        if(bytes_received >= 1500) 
        {
            printf("NetClient Warning: Packet received was over 1500 bytes!  %i bytes\n ", bytes_received);
        }
        if(validate_packet(buffer, bytes_received, &from)) {
            process_packet(buffer, bytes_received);
        }

        //force flush to make sure acks get through during stalls


        NPserver.received_since_last_send++;
        if( NPserver.received_since_last_send >= 8)
        {
            NPserver.flush_to_net();
            printf("client.process_incoming_packets: force flush ack, more than 8 packets received between acks \n");
        }
    }
}

int header_size1() {
    return sizeof(uint8_t)+3*sizeof(uint16_t)+ 2*sizeof(uint32_t);
}

void process_packet(char* buff, int n) {
    if(n==6) return;

    int n1=0;

    uint16_t packet_size;
    uint16_t client_id;
    uint16_t sequence_number;

    uint16_t max_seq;
    uint32_t acks;

    n1=0;

    UNPACK_uint16_t(&client_id, buff, &n1); //client id

    if(client_id != NPserver.client_id) {
        printf("Client ID is wrong: %i, %i\n", client_id, NPserver.client_id);
        return;
    }

    UNPACK_uint16_t(&packet_size, buff, &n1);         //channel 1
    UNPACK_uint16_t(&sequence_number, buff, &n1);   //sequence number
    UNPACK_uint16_t(&max_seq, buff, &n1);           //max seq
    UNPACK_uint32_t(&acks, buff, &n1);              //sequence number
    
    //printf("sequence number= %i \n", sequence_number);
    
    process_acks(&NPserver, max_seq, acks);
    set_ack_for_received_packet(&NPserver ,sequence_number);
    NPserver.last_packet_time = get_current_netpeer_time();
    NPserver.ttl = NPserver.ttl_max; //increase ttl if packet received

    process_packet_messages(buff, n1, n, client_id);
}


int poll_connection_timeout() {
    NPserver.ttl -= 1;
    //printf("np: %i %i\n",NPserver.last_packet_time,get_current_netpeer_time() );
    //printf("np_delta=%i\n", NP_time_delta1(NPserver.last_packet_time));
    if(NP_time_delta1(NPserver.last_packet_time) > 4000 && NPserver.connected == 1) {
        NPserver.connected = 0;
        printf("=== \nConnection to server timed out!\n=== \n");
        client_disconnect_event(0);
        return 1;
    }
    return 0;
}

/*
int decrement_ttl() {
    NPserver.ttl -= 1;
    if(NPserver.ttl <= 0) {
        NPserver.connected = 0;
        printf("===\nConnection to server timed out!\n====\n");
        client_disconnect_event(0);
        return 1;
    }
    return 0;
}
*/

}
