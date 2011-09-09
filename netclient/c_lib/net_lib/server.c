#include "server/server.h"

#include "server.h"

//#include "common/net_packets.h"

#include "common/net_peer.c"
#include "server/server.c"
#include "common/sequencer.c"
#include "common/message_handler.c"
  //should be in header?

void _NetServerInit() {
    update_current_netpeer_time();
    init_net_lib();
    init_message_handler();

    unsigned short port = 9999;
    init_server(port);

}

long t1 = 0;
long t2 = 0;

void _NetServerTick() {
    //printf("tick\n");
    if(0) {
        t2 = t1;
        t1 = _get_time();
        printf("delta: %i\n",t1 - t2);
    }
        update_current_netpeer_time();
        //NP_print_delta();

        process_packets();
        broad_cast_packet2();

        check_pool_for_dropped_packets();
        //decrement_ttl();
        poll_connection_timeout();

}
