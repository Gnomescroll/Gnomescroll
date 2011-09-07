#include "client.h"


#include "common/net_packets.h"

#include "common/net_peer.c"
#include "client/client.c"
#include "common/sequencer.c"


struct NetPeer* np;

void _NetClientConnect(int a, int b,int c, int d, int _port) {

    init_client();

    unsigned int port = 9999;
    set_server(a,b,c,d, port);
    attempt_connection_with_server();

    np= CLIENT_get_NP();
}

/*
Notes:
-sample keyboard state input at constant rate, 60 FPS: Detect input changes early
-seperate out packet out routine from listener.  seperate listener from input sampler and transmission
-short circuit send packet for events (firing,jumping)
-read input into buffer and have physics do ticks from buffer

physics tick, control input sample ticks and netout ticks should be seperate loops
*/

void _NetClientTick() {

    process_incoming_packets();
    send_packet2();
    check_for_dropped_packets(np);
    decrement_ttl();

    return;
}
