#include "server/server.h"

#include "server.h"

#include "common/net_packets.h"

#include "common/net_peer.c"
#include "server/server.c"
#include "common/sequencer.c"


void _NetServerInit() {

    init_net_lib();

    unsigned short port = 9999;
    init_server(port);

}

void _NetServerTick() {

        update_current_netpeer_time();
        NP_print_delta();

        process_packets();
        broad_cast_packet2();

        check_pool_for_dropped_packets();
        //decrement_ttl();
        poll_connection_timeout();
    }
