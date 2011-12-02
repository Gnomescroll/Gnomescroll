#include <net_lib/client.h>
#ifdef DC_CLIENT

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <time/physics_timer.h>


#include <net_lib/benchmark.hpp>

#include <net_lib/common/message_handler.h>
#include <net_lib/client/client.hpp>




#include <c_lib/state/packet_init.cpp>
#include <c_lib/state/client_state.cpp>



#include <net_lib/benchmark.hpp>
#include <stdlib.h>

void _net_test() {
    //return;

    int start1 = _GET_MS_TIME();
    int start =  _GET_MS_TIME();
    int c = 0;
    while(1) 
    {
        c++;
        benchmarkCtoS a;
        a.id = 0;
        a.seq = 5;
        a.cs = 15;
        a.theta = 15.60;
        a.phi = 243.0;
        int i;
        for(i=0; i < 75; i++) a.send(); //70

        int ti= _GET_MS_TIME();
        if(ti - start > 5000) 
        {
            printf("%i packets in %i ms, %i per second \n", c, ti - start, c/5);
            start = ti;
            c = 0;
        }
        NetClient::NPserver.flush_to_net();

        if(ti -start1 > 20000) {
            exit(0);
            return;
        }
    }
}

void _test_reliable() {
    
    static int _r_sent = 0;

    if(_r_sent > 1000) return;

    benchmarkReliableCtoS a;
    a.id = 0;
    a.seq = 5;
    a.cs = 15;
    a.theta = 15.60;
    a.phi = 243.0;
    
    a.send();

    _r_sent++;
    printf("sent %i messages\n", _r_sent);

}

class NetPeer* np;

void _NetClientConnect(int a, int b,int c, int d, int _port) {
    init_message_handler();
    PacketInit::RegisterMessages();
    ClientState::InitClientState();

    update_current_netpeer_time();
    NetClient::init_client();

    unsigned int port = 9999;
    NetClient::set_server(a,b,c,d, port);
    //set_server(127,0,0,1, port);
    //set_server(a,b,c,d, port);
    NetClient::attempt_connection_with_server();

    np= NetClient::CLIENT_get_NP();

}

    /*
    Notes:
    -sample keyboard state input at constant rate, 60 FPS: Detect input changes early
    -seperate out packet out routine from listener.  seperate listener from input sampler and transmission
    -short circuit send packet for events (firing,jumping)
    -read input into buffer and have physics do ticks from buffer

    physics tick, control input sample ticks and netout ticks should be seperate loops
    */

int _N =0;

void _NetClientStartFrame() {

    update_current_netpeer_time();
    //start physics frame
    pviz_start_frame();

    //connection management stuff
    _N++;

    //connection timeout returns if connected and times out on this frame

    if(NetClient::poll_connection_timeout() == 1) _N=0; //by setting _N to 0, will attempt to reconnect immediately on disconnect

    if(np->connected == 0) {
        if(_N % 90 == 0) printf("UDP Socket not connected!\n");
        if(_N % 90 == 0) NetClient::attempt_connection_with_server();
        return;
    } else {
        //_test_reliable();
        //_net_test(); //benchmark
    }

    //deal with retransmission before retransmission
    check_for_dropped_packets(np);
}

void _NetClientStateTick() {
    return;
    //Does nothing
    ClientState::ClientTick();

}

void _NetClientNetInTick() {

    //update time before processing packets
    update_current_netpeer_time();
    NetClient::process_incoming_packets();

}

void _NetClientNetOutTick() {


    update_current_netpeer_time();

    //does not do anything
    //ClientState::send_control_state();

    check_for_dropped_packets(np);      //check again, probably redundant
    //NetClient::flush_outgoing_packets();
    NetClient::NPserver.flush_to_net();
    
    NetClient::poll_connection_timeout();

    ClientState::ClientTick();

}


#endif
