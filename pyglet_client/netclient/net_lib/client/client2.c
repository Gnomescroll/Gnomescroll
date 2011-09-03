#include "client2.h"

struct NetClient server;

struct timeval timeout;
fd_set read_flags;
fd_set write_flags;

unsigned char buffer[1500]; //1500 is max ethernet MTU

void init_client() {
    server.client_id = 65535;
    server.connected =0;
    server.local_port = 6967;
    server.ttl = 0;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    server.socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( server.socket <= 0 )
    {
        printf( "failed to create socket\n" );
        return;
    }
    //bind socket

    //sockaddr_in address;
    server.local_address.sin_family = AF_INET;
    server.local_address.sin_addr.s_addr = INADDR_ANY;
    server.local_address.sin_port = htons( (unsigned short) server.local_port ); //set port 0 for any port


    if ( bind( server.socket, (const struct sockaddr*) &server.local_address, sizeof(struct sockaddr_in) ) < 0 )
    {
        printf( "failed to bind socket\n" );
        return;
    }
    printf("Socket bound to port %i\n", server.local_port);

    //set socket to non-blocking

    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

        int nonBlocking = 1;
        if ( fcntl( server.socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
        {
            printf( "failed to set non-blocking socket\n" );
            return;
        }

    #elif PLATFORM == PLATFORM_WINDOWS

        DWORD nonBlocking = 1;
        if ( ioctlsocket( server.socket, FIONBIO, &nonBlocking ) != 0 )
        {
            printf( "failed to set non-blocking socket\n" );
            return;
        }
    #endif
}

void send_packet(unsigned char* buff, int n){
    if(server.connected == 0) {
        printf("Cannot send packet, disconnected!\n");
        }
    int sent_bytes = sendto( server.socket, (const char*)buff, n,0, (const struct sockaddr*)&server.server_address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != n) { printf( "failed to send packet: return value = %i of %i\n", sent_bytes, n );return;}
}

void attempt_connection_with_server() {
    unsigned char buff[6];
    int n=0;
    PACK_uint16_t(0, buff, &n);
    PACK_uint8_t(255, buff, &n);
    n=6; //must be 6 bytes
    sendto( server.socket, (const char*)buff, n,0, (const struct sockaddr*)&server.server_address, sizeof(struct sockaddr_in) );
    printf("Client id requested\n");
}

int select_return;

void set_server(int a, int b, int c, int d, unsigned short port) {
    server.server_ip =  ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
    server.server_port = port;

    server.server_address.sin_family = AF_INET;
    server.server_address.sin_addr.s_addr = htonl(server.server_ip);
    server.server_address.sin_port = htons(server.server_port);

}


int validate_packet(unsigned char* buff, int n, struct sockaddr_in* from) {
    //check CRC
    if(from->sin_addr.s_addr != server.server_address.sin_addr.s_addr) {
        unsigned int from_address = ntohl( from->sin_addr.s_addr );
        unsigned short from_port = ntohs( from->sin_port );
        printf("Received rogue %i byte packet from IP= %i:%i  Server IP = %i:%i\n", n,from_address,from_port, ntohl(server.server_address.sin_addr.s_addr), ntohs(server.server_address.sin_port));
        return 0;
    }
    if(server.connected == 0 && n ==6) {
        int n1=0;
        unsigned short message_id, client_id;
        UNPACK_uint16_t(&message_id, buff, &n1);
        UNPACK_uint16_t(&client_id, buff, &n1);
    }
    return 1;
}

void process_incoming_packets() {
    unsigned int n=0;
    struct sockaddr_in from;

    //FD_ZERO(&read_flags); // Zero the flags ready for using
    //FD_SET(server.socket, &read_flags);

    FD_ZERO(&read_flags); // Zero the flags ready for using
    FD_SET(server.socket, &read_flags);

    while(1) {

        select_return = select(server.socket+1, &read_flags,(fd_set*)0,(fd_set*)0,&timeout);
        if (select_return < 0) {
            printf("select returned %i\n", select_return);break;
        }

        if (FD_ISSET(server.socket, &read_flags)) {
            printf("Incoming Packet\n");
            FD_CLR(server.socket, &read_flags);
            //get packets
            n = recvfrom(server.socket, buffer, 1500, 0, (struct sockaddr*)&from, &n);
            if(!validate_packet(buffer, n, &from)) {
            printf("Received %i bytes\n", n);
            process_packet(buffer, n);
        }} else {
            break;
        }

}
}

void process_packet(unsigned char* buf, int n) {
    int n1=0;
    if(n==6) {
        uint16_t client_id;
        uint16_t channel_id;
        UNPACK_uint16_t(&channel_id, buf, &n1);
        UNPACK_uint16_t(&client_id, buf, &n1);
        printf("Received client id= %i\n",client_id);
    }
}

void process_outgoing_packets() {

    FD_ZERO(&write_flags);
    //if(no data to send) { return }
    FD_SET(server.socket, &write_flags); //if need to send data, check
    select(server.socket+1, (fd_set*)0, &write_flags, (fd_set*)0, &timeout);
    if (FD_ISSET(server.socket, &write_flags)) {
        FD_CLR(server.socket, &write_flags);
        printf("Socket ready to send packets\n");
    }
    return;

}
