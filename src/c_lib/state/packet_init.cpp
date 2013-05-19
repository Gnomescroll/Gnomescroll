#include "packet_init.hpp"

#include <agent/net_agent.hpp>
#include <t_map/_interface.hpp>
#include <item/_interface.hpp>
#include <item/toolbelt/_interface.hpp>
#include <item/container/_interface.hpp>
#include <item/particle/_interface.hpp>
#include <particle/grenade.hpp>
#include <auth/_interface.hpp>
#include <social/badges.hpp>
#include <common/dat/attributes.hpp>

namespace PacketInit
{

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

    agent_state_StoC::register_client_packet();
    agent_teleport_StoC::register_client_packet();

    agent_camera_state_CtoS::register_server_packet();

    send_client_id_StoC::register_client_packet();

    agent_control_state_CtoS::register_server_packet();
    agent_control_state_StoC::register_client_packet();

    // agent took damage indicator
    agent_dead_StoC::register_client_packet();
    agent_create_StoC::register_client_packet();
    agent_destroy_StoC::register_client_packet();
    client_disconnected_StoC::register_client_packet();

    agent_set_block_CtoS::register_server_packet();

    // particle system messages
    Particle::grenade_StoC::register_client_packet();
    Particle::plasmagen_explode_StoC::register_client_packet();

    // agent hit block action
    hit_block_CtoS::register_server_packet();

    //player agent
    player_agent_snapshot_StoC::register_client_packet();

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

    player_agent_id_StoC::register_client_packet();

    throw_grenade_CtoS::register_server_packet();

    agent_kills_StoC::register_client_packet();
    agent_deaths_StoC::register_client_packet();
    agent_suicides_StoC::register_client_packet();

    // chat
    Chat::init_packets();

    //ping
    ping_CtoS::register_server_packet();
    ping_StoC::register_client_packet();
    ping_reliable_CtoS::register_server_packet();
    ping_reliable_StoC::register_client_packet();

    agent_conflict_notification_StoC::register_client_packet();

    // init phase
    Entities::init_packets();
    Item::init_packets();
    Toolbelt::init_packets();
    ItemContainer::init_packets();
    ItemParticle::init_packets();

    // Map Messages

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

    teleport_me_CtoS::register_server_packet();

    gohome_CtoS::register_server_packet();

    Badges::init_packets();
    Attributes::init_packets();
}

}   // PacketInit
