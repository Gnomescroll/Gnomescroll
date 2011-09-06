#include "server/server.c"
#include "client/sequence_numbers.c"

#include "net_packets.h"

/*
struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
struct Socket create_socket(uint32_t IP, uint16_t  port); //port=0 to get any port
int send_packet(struct Socket* socket, struct NetPeer* p, char* packet_data, int packet_size)

void receive_packets(struct* Socket socket);
*/

#include <unistd.h>

int main() {

    init_net_lib();

    unsigned short port = 9999;
    init_server(port);

    int i=0;
    while(1) {

        printf("* ------ tick=%i\n", i);

        process_packets();
        broad_cast_packet();
        //broad_cast_packet();
        decrement_ttl();
        sleep(1);
        i++;
    }

    return 0;
}
