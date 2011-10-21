#include <net_lib/client.h>

#include <net_lib/common/message_handler.h>
#include <net_lib/client/client.hpp>




#include <c_lib/state/packet_init.cpp>
#include <c_lib/state/client_state.cpp>


struct NetPeer* np;

void _NetClientConnect(int a, int b,int c, int d, int _port) {
    NetClient::reset_client_out_buffer();
    //init_agent_client();
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
void _NetClientTick() {


    //Agent_control_state_message acs;
    //acs.send();

    //printf("net client tick\n");
    update_current_netpeer_time();
    pviz_start_frame();
    //NP_print_delta();
    _N++;
    //NP_time_delta1(np.last_packet_time) //time since last packet
    //pviz_start_frame();
    NetClient::process_incoming_packets();
    //NP_time_delta1(np.last_packet_time)
    NetClient::poll_connection_timeout();
    if(np->connected == 0) {
        if(_N % 90 == 0) printf("UDP Socket not connected!\n");
        if(_N % 90 == 0) NetClient::attempt_connection_with_server();
        return;
    }

    //set agent state
    ClientState::send_control_state();

    NetClient::flush_outgoing_packets();
    check_for_dropped_packets(np);
    NetClient::poll_connection_timeout();

    ClientState::ClientTick();
    return;
}
