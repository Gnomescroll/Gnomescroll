#include "./packet_init.hpp"

#include <agent/net_agent.hpp>
#if DC_CLIENT
#include <agent/client/player_agent.hpp>
#endif


#include <t_map/_interface.hpp>

#include <item/_interface.hpp>
#include <item/toolbelt/_interface.hpp>
#include <item/container/_interface.hpp>
#include <item/particle/_interface.hpp>

#include <particle/grenade.hpp> // move into net folder in particles
#include <auth/_interface.hpp>

namespace PacketInit
{

typedef void (*pt2handler)(unsigned char*, int, int* read_bytes);

void RegisterMessages()
{
    //version (keep these first in the list)
    version_CtoS::register_server_packet();
    version_StoC::register_client_packet();

/*
    add back in 
    benchmarkCtoS::register_server_packet();    //packet for benchmarking
    benchmarkReliableCtoS::register_server_packet();  
*/  

    //Agent_control_state_message::register_server_packet();
    Agent_state_message::register_client_packet();
    Agent_teleport_message::register_client_packet();
    //Agent_control_state_to_client_message::register_client_packet();

    agent_camera_state_CtoS::register_server_packet();

    SendClientId_StoC::register_client_packet();
    
    Agent_cs_CtoS::register_server_packet();
    Agent_cs_StoC::register_client_packet();

    // agent took damage indicator
    agent_damage_StoC::register_client_packet();
    // player health update
    agent_health_StoC::register_client_packet();
    agent_dead_StoC::register_client_packet();
    agent_create_StoC::register_client_packet();
    agent_destroy_StoC::register_client_packet();
    agent_name_StoC::register_client_packet();
    request_agent_name_CtoS::register_server_packet();
    client_disconnected_StoC::register_client_packet();
    
    agent_set_block_CtoS::register_server_packet();
    //#if !PRODUCTION
    admin_set_block_CtoS::register_server_packet();
    //#endif

    // particle system messages
    Particle::grenade_StoC::register_client_packet();
 
    // agent hit block action
    hit_block_CtoS::register_server_packet();

    //player agent
    PlayerAgent_Snapshot::register_client_packet();

    // hitscans
    hitscan_object_CtoS::register_server_packet();
    hitscan_block_CtoS::register_server_packet();
    hitscan_none_CtoS::register_server_packet();

    // melees
    melee_object_CtoS::register_server_packet();
    melee_none_CtoS::register_server_packet();

    // hitscan StoC action (for animations etc)
    agent_shot_object_StoC::register_client_packet();
    agent_shot_block_StoC::register_client_packet();
    agent_shot_nothing_StoC::register_client_packet();
    agent_melee_object_StoC::register_client_packet();
    agent_melee_nothing_StoC::register_client_packet();

    agent_hit_block_StoC::register_client_packet();
    agent_threw_grenade_StoC::register_client_packet();
    agent_placed_block_StoC::register_client_packet();

    // agent item actions
    place_spawner_CtoS::register_server_packet();
    choose_spawner_CtoS::register_server_packet();
    set_spawner_StoC::register_client_packet();
    place_turret_CtoS::register_server_packet();

    PlayerAgent_id_StoC::register_client_packet();

    ThrowGrenade_CtoS::register_server_packet();

    AgentKills_StoC::register_client_packet();
    AgentDeaths_StoC::register_client_packet();
    AgentSuicides_StoC::register_client_packet();

    //// identification
    identify_CtoS::register_server_packet();
    identified_StoC::register_client_packet();

    // chat
    ChatMessage_CtoS::register_server_packet();
    ChatMessage_StoC::register_client_packet();

    //ping
    ping_CtoS::register_server_packet();
    ping_StoC::register_client_packet();
    ping_reliable_CtoS::register_server_packet();
    ping_reliable_StoC::register_client_packet();

    agent_conflict_notification_StoC::register_client_packet();

    //destroy_voxel_StoC::register_client_packet();

    /*
     * Objects
     */

    // object crud
    object_create_StoC::register_client_packet();

    object_create_owner_StoC::register_client_packet();

    object_create_momentum_StoC::register_client_packet();
    object_create_momentum_angles_StoC::register_client_packet();
    object_create_momentum_angles_health_StoC::register_client_packet();
    
    object_destroy_StoC::register_client_packet();

    object_state_StoC::register_client_packet();
    object_state_momentum_StoC::register_client_packet();
    object_state_momentum_angles_StoC::register_client_packet();
    object_state_health_StoC::register_client_packet();
    
    // object actions
    object_picked_up_StoC::register_client_packet();

    // object fired
    object_shot_object_StoC::register_client_packet();
    object_shot_terrain_StoC::register_client_packet();
    object_shot_nothing_StoC::register_client_packet();

    // object/monster motion
    object_choose_weapon_target_StoC::register_client_packet();
    object_choose_motion_target_StoC::register_client_packet();
    object_remove_motion_target_StoC::register_client_packet();
    object_choose_destination_StoC::register_client_packet();

    // damage
    object_took_damage_StoC::register_client_packet();

    // bomb state machine
    object_begin_waiting_StoC::register_client_packet();
    object_in_transit_StoC::register_client_packet();
    object_chase_agent_StoC::register_client_packet();

    /* init phase */

    request_remaining_state_CtoS::register_server_packet();

    Item::init_packets();
    Toolbelt::init_packets();
    ItemContainer::init_packets();
    ItemParticle::init_packets();

    /*
        Map Messages
    */
    
    t_map::init_packets();
    t_mech::init_packets();
    // suicide
    killme_CtoS::register_server_packet();
    
    // coloring
    colorme_CtoS::register_server_packet();
    agent_color_StoC::register_client_packet();

    // sound
    Sound::init_packets();

    // animations
    Animations::init_packets();

    // auth
    Auth::init_packets();

}

}
