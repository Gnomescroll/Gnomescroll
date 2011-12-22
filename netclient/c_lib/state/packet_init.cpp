#include "./packet_init.hpp"

#include <net_lib/common/message_handler.h>
#include <c_lib/agent/net_agent.hpp>
#include <c_lib/agent/player_agent.hpp>

#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/t_map/t_map.hpp>

#include <c_lib/game/packets.hpp>

namespace PacketInit {
typedef void (*pt2handler)(unsigned char*, int, int* read_bytes);

void RegisterMessages() {

    benchmarkCtoS::register_server_packet();    //packet for benchmarking
    benchmarkReliableCtoS::register_server_packet();  
    
    //Agent_control_state_message::register_server_packet();
    Agent_state_message::register_client_packet();
    Agent_teleport_message::register_client_packet();
    //Agent_control_state_to_client_message::register_client_packet();

    Agent_cs_CtoS::register_server_packet();
    Agent_cs_StoC::register_client_packet();

    // grenade creation
    grenade_StoC::register_client_packet();

    // agent took damage indicator
    agent_damage_StoC::register_client_packet();
    // player health update
    agent_health_StoC::register_client_packet();
    agent_dead_StoC::register_client_packet();
    agent_create_StoC::register_client_packet();

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

    // game/teams stuff
    TeamColor_StoC::register_client_packet();
    //TeamName_StoC::register_client_packet();
    AgentJoinTeam_StoC::register_client_packet();
    AgentJoinTeam_CtoS::register_server_packet();

    AgentAutoAssignTeam_CtoS::register_server_packet();

    PlayerAgent_id_StoC::register_client_packet();

}
}
