#pragma once

#include <packet.hpp>
#include <packets.hpp>
#include <net_lib/message_handler.h>
#include <net_lib/benchmark.hpp>

void register_packets()
{
    init_message_handler();
    printf("Registering Network Packet Callbacks\n");
    //to client
    TestToClient::register_client_packet();

    ClientID_StoC::register_client_packet();
    AgentCreate_StoC::register_client_packet();
    AgentTeleport_StoC::register_client_packet();
    SetBlock_StoC::register_client_packet();
    ProjectileCreate_StoC::register_client_packet();
    AgentDestroy_StoC::register_client_packet();

    AgentState_StoC::register_client_packet();
    //to server

    TestToServer::register_server_packet();

    benchmarkCtoS::register_server_packet();
    
    AgentState_CtoS::register_server_packet();
    SetBlock_CtoS::register_server_packet();
    AgentFire_CtoS::register_server_packet();

}
