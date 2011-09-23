

#include "server.h"

/*
void _NetServerRegisterMessages() {
    Agent_control_state_message_register_message(); //3
    //Agent_state_message_register_message();         //4
}
*/

#include <c_lib/agent/agent_include.h>

#include <c_lib/state/server_init.cpp>
/*
class AgentList agent_list;

void _NetServerInitState() {
    int id = agent_list.new_agent();
    printf("new agent id= %i\n", id);
}
*/

void _NetServerInit() {
    init_message_handler();
    update_current_netpeer_time();
    init_net_lib();
    init_message_handler();

    unsigned short port = 9999;
    init_server(port);

    ServerInit::RegisterMessages();
}

void _NetServerStateTick() {

}

long t1 = 0;
long t2 = 0;

void _NetServerTick() {
    if(0) {
        t2 = t1;
        t1 = _get_time();
        printf("delta: %i\n",(int)(t1 - t2));
    }
        update_current_netpeer_time();
        //NP_print_delta();

        process_packets(); //should poll for packets very often; event triggered packet dump

        _NetServerStateTick();

        broad_cast_packet2();

        check_pool_for_dropped_packets();
        //decrement_ttl();
        poll_connection_timeout();

}
