#include "./packet_init.hpp"

//#include <net_lib/common/message_handler.h>

#include <game/packets.hpp>

#include <agent/net_agent.hpp>
#include <agent/client/player_agent.hpp>

#include <t_map/net/t_CtoS.hpp>
#include <t_map/net/t_StoC.hpp>

#include <particle/net/CtoS.hpp>
#include <particle/net/StoC.hpp>

#include <item/net/CtoS.hpp>
#include <item/net/StoC.hpp>

#include <particle/grenade.hpp> // move into net folder in particles
namespace PacketInit {
typedef void (*pt2handler)(unsigned char*, int, int* read_bytes);

void RegisterMessages() {

/*
    add back in 
    benchmarkCtoS::register_server_packet();    //packet for benchmarking
    benchmarkReliableCtoS::register_server_packet();  
*/  

    //Agent_control_state_message::register_server_packet();
    Agent_state_message::register_client_packet();
    Agent_teleport_message::register_client_packet();
    //Agent_control_state_to_client_message::register_client_packet();

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

    // particle system messages
    Particle::grenade_StoC::register_client_packet();
    Particle::item_particle_create_StoC::register_client_packet();
    Particle::item_particle_destroy_StoC::register_client_packet();
    Particle::item_particle_picked_up_StoC::register_client_packet();

    /* item*/
/*
    item::item_create_StoC::register_client_packet();;
    
    item::assign_agent_inventory_StoC::register_client_packet();
    item::assign_agent_toolbelt_StoC::register_client_packet();
    item::assign_agent_nanite_StoC::register_client_packet();

    item::swap_within_inventory_StoC::register_client_packet();
    item::swap_between_inventory_StoC::register_client_packet();
*/
    /*
        Map Messages
    */
    
    t_map::map_chunk_compressed_StoC::register_client_packet();
    t_map::map_chunk_uncompressed_StoC::register_client_packet();

    t_map::set_map_alias_StoC::register_client_packet();
    t_map::clear_alias_StoC::register_client_packet();

    // block value change
    t_map::block_StoC::register_client_packet();
    t_map::block_action_StoC::register_client_packet();
    t_map::block_CtoS::register_server_packet();
    t_map::map_metadata_StoC::register_client_packet();
    
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

    alter_item_ownership_StoC::register_client_packet();
    
    // agent item actions
    place_spawner_CtoS::register_server_packet();
    choose_spawn_location_CtoS::register_server_packet();
    spawn_location_StoC::register_client_packet();
    place_turret_CtoS::register_server_packet();

    // game/teams stuff
    TeamColor_StoC::register_client_packet();
    TeamScore_StoC::register_client_packet();
    TeamName_StoC::register_client_packet();
    AgentJoinTeam_StoC::register_client_packet();
    AgentJoinTeam_CtoS::register_server_packet();
    AgentAutoAssignTeam_CtoS::register_server_packet();

    FlagState_StoC::register_client_packet();
    BaseState_StoC::register_client_packet();

    AgentPickupFlag_StoC::register_client_packet();
    AgentDropFlag_StoC::register_client_packet();
    AgentScoreFlag_StoC::register_client_packet();

    // coins
    agent_coins_StoC::register_client_packet();
    
    PlayerAgent_id_StoC::register_client_packet();

    ThrowGrenade_CtoS::register_server_packet();

    AgentKills_StoC::register_client_packet();
    AgentDeaths_StoC::register_client_packet();
    AgentSuicides_StoC::register_client_packet();

    AgentActiveWeapon_CtoS::register_server_packet();
    AgentActiveWeapon_StoC::register_client_packet();
    AgentReloadWeapon_CtoS::register_server_packet();
    AgentReloadWeapon_StoC::register_client_packet();

    Weapons::WeaponAmmo_StoC::register_client_packet();
    Weapons::WeaponClip_StoC::register_client_packet();

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

    //version
    version_StoC::register_client_packet();

    agent_conflict_notification_StoC::register_client_packet();

    destroy_voxel_StoC::register_client_packet();

    /*
     * Objects
     */

    // object crud
    object_create_StoC::register_client_packet();
    object_create_owner_team_StoC::register_client_packet();
    object_create_owner_team_index_StoC::register_client_packet();

    object_create_momentum_StoC::register_client_packet();
    object_create_momentum_angles_StoC::register_client_packet();
    
    object_destroy_StoC::register_client_packet();

    object_state_StoC::register_client_packet();
    object_state_momentum_StoC::register_client_packet();
    object_state_momentum_angles_StoC::register_client_packet();
    
    // object actions
    object_picked_up_StoC::register_client_packet();

    // object fired
    object_shot_object_StoC::register_client_packet();
    object_shot_terrain_StoC::register_client_packet();
    object_shot_nothing_StoC::register_client_packet();

    // object/monster motion
    object_choose_target_StoC::register_client_packet();
    object_choose_destination_StoC::register_client_packet();


    /* init phase */

    request_remaining_state_CtoS::register_server_packet();

     //inventory
    /*
    inventory_create_StoC::register_client_packet();
    inventory_destroy_StoC::register_client_packet();
    add_item_to_inventory_StoC::register_client_packet();
    remove_item_from_inventory_StoC::register_client_packet();
    swap_item_in_inventory_StoC::register_client_packet();
    swap_item_between_inventory_StoC::register_client_packet();
    merge_stack_in_inventory_StoC::register_client_packet();
    merge_stack_between_inventory_StoC::register_client_packet();
    set_stack_inventory_StoC::register_client_packet();
    */

    // client actions
    /*
    add_item_to_inventory_CtoS::register_server_packet();
    remove_item_from_inventory_CtoS::register_server_packet();
    swap_item_in_inventory_CtoS::register_server_packet();
    swap_item_between_inventory_CtoS::register_server_packet();
    merge_stack_in_inventory_CtoS::register_server_packet();
    merge_stack_between_inventory_CtoS::register_server_packet();
    */
}

}
