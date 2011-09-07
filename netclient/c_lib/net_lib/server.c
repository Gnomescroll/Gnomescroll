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


void _NetServerInit() {

    init_net_lib();

    unsigned short port = 9999;
    init_server(port);

}

void _NetServerTick() {

        process_packets();

        broad_cast_packet();

        check_pool_for_dropped_packets();
        decrement_ttl();
    }
