#include <unistd.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "server/server.h"
#include "common/net_packets.h"
#include "common/sequencer.h"

//c body includes
#include "common/sequencer.c"
#include "common/net_peer.c"
#include "server/server.c"


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
