#include <unistd.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "server/server.h"
#include "common/sequencer.h"

//c body includes
#include "common/sequencer.c"
#include "common/net_peer.c"
#include "server/server.c"
#include "common/message_handler.c"

//#include "../time/physics_timer.c"


void _NetServerInit() {
    update_current_netpeer_time();
    init_net_lib();
    init_message_handler();

    unsigned short port = 9999;
    init_server(port);

}

void _NetServerTick() {
    //printf("tick\n");

        update_current_netpeer_time();
        //NP_print_delta();

        process_packets();
        broad_cast_packet2();

        check_pool_for_dropped_packets();
        //decrement_ttl();
        poll_connection_timeout();
}


int main() {
    _START_CLOCK();
    _NetServerInit();
    int i =0;
    while(1) {
        i++;
        printf("tick=%i\n",i);
        _NetServerTick();
        sleep(1);
    }
}
