#include "server.c"


/*
struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
struct Socket create_socket(uint32_t IP, uint16_t  port); //port=0 to get any port
int send_packet(struct* Socket socket, char* packet_data, int packet_size);

void receive_packets(struct* Socket socket);
*/

int main() {

    struct Socket* s;
    s = create_socket(0, 0);

    receive_packets(s);

    return 0;
}
