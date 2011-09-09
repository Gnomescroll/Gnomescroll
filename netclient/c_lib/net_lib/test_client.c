
#include <unistd.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "client/client.h"

#include "common/sequencer.h"
#include "common/message_handler.h"

//c body includes
#include "common/sequencer.c"
#include "common/net_peer.c"
#include "common/message_handler.c"

#include "client/client.c"

//#include "../time/physics_timer.c"


struct NetPeer* np;

void _NetClientConnect(int a, int b,int c, int d, int _port) {
    update_current_netpeer_time();
    init_client();
    init_message_handler();

    unsigned int port = 9999;
    set_server(127,0,0,1, port);
    //set_server(a,b,c,d, port);
    attempt_connection_with_server();

    np= CLIENT_get_NP();
}

int _N =0;
void _NetClientTick() {
    //printf("net client tick\n");
    update_current_netpeer_time();
    //NP_print_delta();
    _N++;

    process_incoming_packets();
    poll_connection_timeout();
    /*
    if(np->connected == 0) {
        if(np->client_id < 500) {
            printf("==\n client_id= %i\n===\n", np->client_id);
        }
        if(_N % 90 == 0) printf("UDP Socket not connected!\n");
        if(_N % 90 == 0) attempt_connection_with_server();
        return;
    }
    */
    send_packet2();
    check_for_dropped_packets(np);
    //poll_connection_timeout();
    //decrement_ttl();

    return;
}


int main() {
    _START_CLOCK();
    _NetClientConnect(127,0,0,1, 9999);

    while(1) {
        _NetClientTick();
        sleep(1);
    }
}
