struct NetClient {
    uint16_t client_id;
    int ttl; //for connection
    int connected;

    int socket;

    struct sockaddr_in local_address;
    uint16_t local_port;

    struct sockaddr_in server_address;
    uint32_t server_ip;
    uint16_t server_port;
};

struct NetClient server;

struct timeval timeout;
fd_set read_flags;
fd_set write_flags;

void init_client() {
    client_id = 65535;
    server.connected =0;
    server.local_port = 6967;
    server.ttl = 0;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    server->socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( s->socket <= 0 )
    {
        printf( "failed to create socket\n" );
        return;
    }
    //bind socket

    //sockaddr_in address;
    server->local_address.sin_family = AF_INET;
    server->local_address.sin_addr.s_addr = INADDR_ANY;
    server->local_address.sin_port = htons( (unsigned short) server.local_port ); //set port 0 for any port


    if ( bind( server->socket, (const struct sockaddr*) &server->local_address, sizeof(struct sockaddr_in) ) < 0 )
    {
        printf( "failed to bind socket\n" );
        return;
    }
    printf("Socket bound to port %i\n", server.local_port);

    //set socket to non-blocking

    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

        int nonBlocking = 1;
        if ( fcntl( server->socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
        {
            printf( "failed to set non-blocking socket\n" );
            return;
        }

    #elif PLATFORM == PLATFORM_WINDOWS

        DWORD nonBlocking = 1;
        if ( ioctlsocket( server->socket, FIONBIO, &nonBlocking ) != 0 )
        {
            printf( "failed to set non-blocking socket\n" );
            return;
        }

}

void send_packet(unsigned char* buffer, int n){
    if(server.connected == 0) {
        printf("Cannot send packet, disconnected!\n");
        }
    int sent_bytes = sendto( server->socket, (const char*)buffer, b,0, (const struct sockaddr*)&server->server_address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != packet_size) { printf( "failed to send packet: return value = %d\n", sent_bytes );return;}
}

void attempt_connection_with_server() {
    unsigned char* buffer[6];
    int n=0;
    PACK_uint16_t(0, buffer, &n);
    PACK_uint8_t(255, buffer, &n);
    n=6; //must be 6 bytes
    int sent_bytes = sendto( server->socket, (const char*)buffer, n,0, (const struct sockaddr*)&server->server_address, sizeof(struct sockaddr_in) );
    printf("Client id requested\n");
}

int select_return;

void set_server(int a, int b, int c, int d, unsigned short port) {
    server.server_ip =  ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
    server.server_port = port;

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = htonl(server.server_ip);
    server->address.sin_port = htons(server.server_portd);

}

unsigned char* buffer[1500]; //1500 is max ethernet MTU

validate_packet(unsigned char* buffer, int* n, struct sockaddr*) {
    //check CRC
    if(from->sin_addr.s_addr != server.server_address.sin_addr.s_addr) {
        printf("Received rogue packet from IP= %i  Server IP = %i\n", ntohl(from->sin_addr.s_addr), ntohl(server.server_address.sin_addr.s_addr));
    }
    if(server.connected == 0 && n ==4) {
        UNPACK_uint16_t(0, buffer, &n);
        UNPACK_uint16_t(client_id, buffer, &n);
    }
}

void process_incoming_packets() {
    int n=0;
    struct sockaddr from;

    FD_ZERO(&read_flags); // Zero the flags ready for using
    FD_SET(server.socket, &read_flags);

    while(1) {
        select_return = select(server.socket+1, &read_flags,(fd_set*)0,(fd_set*)0,&timeout);
        if (select_return < 0) {
            printf("select returned %i\n", select_return);return;
        }

        if (FD_ISSET(thefd, &read_flags)) {
            FD_CLR(thefd, &read_flags);
            //get packets
            n = recvfrom(server.socket, (char*)buffer, 1500, 0, (struct sockaddr*)&from, &n);
            if(!validate_packet(buffer, &n, &from);
            printf("Received %i bytes\n");
            process_packet(buffer, &n);
        } else { break; }
    }
}

process_packet(buffer, &n) {
    int n=0;
    if(n==6) {
        uint16_t = client_id;
        uint16_t = channel_id;
        UNPACK_uint16_t(&client_id, buffer, &n);
        UNPACK_uint8_t(&channel_id, buffer, &n);
        printf("Received client id= %i\n",client_id);
    }
}

void process_outgoing_packets() {
    FD_ZERO(&write_flags);
    //if(no data to send) { return }
    FD_SET(server.socket, &write_flags); //if need to send data, check
    select_return = select(server.socket+1, (fd_set*)0,&write_flags,(fd_set*)0,&timeout);
    if (FD_ISSET(thefd, &write_flags)) {
        FD_CLR(thefd, &write_flags);
        printf("Socket ready to send packets\n");
    }
}
