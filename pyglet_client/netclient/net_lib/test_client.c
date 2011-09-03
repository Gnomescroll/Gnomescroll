//#include "server./server.c"


/*
struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
struct Socket create_socket(uint32_t IP, uint16_t  port); //port=0 to get any port
int send_packet(struct Socket* socket, struct NetPeer* p, char* packet_data, int packet_size)

void receive_packets(struct* Socket socket);
*/

#include "server/server.c"

#include <unistd.h>

/*
struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
struct Socket create_socket(uint32_t IP, uint16_t  port); //port=0 to get any port


int send_packet(struct Socket* socket, struct NetPeer* p, char* packet_data, int packet_size)
*
void receive_packets(struct* Socket socket);
*/

int main() {


    unsigned char* buffer[6];
    int n=0;
    PACK_uint16_t(0, buffer, &n)
    PACK_uint8_t(255, buffer, &n)

    struct Socket* s;
    s = create_socket(0, 9091);
    sleep(1);
    struct NetPeer* p;
    p = create_net_peer(127,0,0,1, 9999);

    printf("send1\n");
    unsigned char data1[4] = {0,0,255,255};
    send_packet(s,p, data1, 4);

    printf("send2\n");
    unsigned char data2[6] = "aaabbb";

    //data2 = *((uint16_t *) packet_data)

    send_packet(s,p, data2, 6);

    //receive_packets(s);

    return 0;
}
