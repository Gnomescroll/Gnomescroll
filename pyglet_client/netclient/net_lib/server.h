#ifndef net_lib_server_h
#define net_lib_server_h

struct Socket {
    uint32_t ip;
    uint16_t port;
    int handle;
    sockaddr_in address;
}

struct NetPeer {
    uint32_t ip;
    uint16_t port;
    sockaddr_in address;
}


struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
struct Socket* create_socket(uint32_t IP, uint16_t  port); //port=0 to get any port
int send_packet(struct* Socket socket, char* packet_data, int packet_size);

void receive_packets(struct* Socket socket);


#endif
