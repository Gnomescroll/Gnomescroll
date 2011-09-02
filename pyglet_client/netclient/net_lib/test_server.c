#include "server/server.c"

#include "net_packets.h"

/*
struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
struct Socket create_socket(uint32_t IP, uint16_t  port); //port=0 to get any port
int send_packet(struct Socket* socket, struct NetPeer* p, char* packet_data, int packet_size)

void receive_packets(struct* Socket socket);
*/

int main() {

    //struct Socket* s;
    //s = create_socket(0, 9090);

    unsigned short port = 9999;
    init_server(port);
    //printf("t\n");
    int i=0;
    while(1) {
        printf("i=%i\n", i);
        process_packets();
        sleep(1);
        i++;
        }
    //receive_packets(s);

    return 0;
}
