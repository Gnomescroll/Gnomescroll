#pragma once
#ifdef DC_SERVER

#include <c_lib/agent/agent_list.hpp>
#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/game/ctf.hpp>
#include <c_lib/monsters/monsters.hpp>
#include <c_lib/voxel/voxel_hitscan.hpp>
#include <c_lib/items/spawner.hpp>

namespace ServerState
{
    extern Agent_list* agent_list GNOMESCROLL_API;
    extern Cspray_list* cspray_list;
    extern Grenade_list* grenade_list GNOMESCROLL_API;
    //extern Neutron_list* neutron_list;
    extern Monsters::Slime_list* slime_list GNOMESCROLL_API;
    extern Voxel_hitscan_list* voxel_hitscan_list;
    extern Spawner_list* spawner_list;
    extern Turret_list* turret_list;
    
    extern CTF* ctf GNOMESCROLL_API;
    
    void init();
    void init_lists();
    void teardown();
    void teardown_lists();

    void damage_objects_within_sphere(
        float x, float y, float z, float radius,
        int dmg, int owner, Object_types inflictor_type
    );

    void server_tick() GNOMESCROLL_API;

    void send_game_state_to_client(int client_id);
    void send_id_to_client(int client_id);
    void send_version_to_client(int client_id);

    void add_player_to_chat(int client_id);
    void remove_player_from_chat(int client_id);

    void send_disconnect_notice(int client_id);

    char* agent_name(int id) GNOMESCROLL_API;

    void start_game() GNOMESCROLL_API;
}

#endif
