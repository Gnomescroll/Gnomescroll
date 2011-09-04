#include "server/server.c"

#include "net_packets.h"

/*
struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
struct Socket create_socket(uint32_t IP, uint16_t  port); //port=0 to get any port
int send_packet(struct Socket* socket, struct NetPeer* p, char* packet_data, int packet_size)

void receive_packets(struct* Socket socket);
*/

#include <unistd.h>

int main() {

    //struct Socket* s;
    //s = create_socket(0, 9090);

/*
    uint8_t d = 5;
    int n= 0;
    unsigned char buf[500];
    printf("i= %i \n", d);
    PACK_uint8_t(1, buf, &n);
    PACK_uint8_t(2, buf, &n);
    n=0;
    UNPACK_uint8_t(&d, buf, &n);
    printf("i= %i \n", d);
    UNPACK_uint8_t(&d, buf, &n);
    printf("i= %i \n", d);
*/
    unsigned short port = 9999;
    init_server(port);
    //printf("t\n");
    int i=0;
    while(1) {
        if(i%5 ==0) {
            printf("tick=%i\n", i);
        }
        process_packets();
        decrement_ttl();
        sleep(1);
        i++;
        }
    //receive_packets(s);

    return 0;
}
