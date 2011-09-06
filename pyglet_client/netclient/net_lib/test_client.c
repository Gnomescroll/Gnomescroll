
#include "net_packets.h"
#include <unistd.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "client/client.c"


int main() {

    init_client();

    unsigned int port = 9999;
    set_server(127,0,0,1, port);
    attempt_connection_with_server();

    struct Pseq* ps = CLIENT_get_Pseq();

    sleep(1);

    int i=0;
    while(1) {

        printf("* ------ tick=%i\n", i);

        process_incoming_packets();
        send_packet2();

        check_for_dropped_packets(ps);
        sleep(1);
        i++;
        }
    return 0;
}
