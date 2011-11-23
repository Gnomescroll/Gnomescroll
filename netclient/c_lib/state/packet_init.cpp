#pragma once

#include <net_lib/common/message_handler.h>
#include <c_lib/agent/net_agent.hpp>
#include <c_lib/agent/player_agent.hpp>

#include <c_lib/particles/object_lib.hpp>
#include <c_lib/t_map/t_map.hpp>

namespace PacketInit {
typedef void (*pt2handler)(unsigned char*, int, int* read_bytes);

void RegisterMessages() {

    //Agent_control_state_message::register_server_packet();
    Agent_state_message::register_client_packet();
    //Agent_control_state_to_client_message::register_client_packet();

    Agent_cs_CtoS::register_server_packet();
    Agent_cs_StoC::register_client_packet();

    // grenade creation
    grenade_StoC::register_client_packet();

    // agent took damage indicator
    agent_damage_StoC::register_client_packet();

    // block value change
    block_StoC::register_client_packet();
    
    // agent hit block action
    hit_block_CtoS::register_server_packet();

    //player agent
    PlayerAgent_Snapshot::register_client_packet();

    //object packets
    cspray_StoC::register_client_packet();

    // fire weapon
    fire_weapon_CtoS::register_server_packet();
    fire_weapon_StoC::register_client_packet();

    // hitscans
    hitscan_agent_CtoS::register_server_packet();
    hitscan_block_CtoS::register_server_packet();

}
}
