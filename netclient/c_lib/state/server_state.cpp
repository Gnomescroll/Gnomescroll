#include "server_state.hpp"
#pragma once
#ifdef DC_SERVER

#include <c_lib/agent/agent.hpp>
#include <c_lib/agent/net_agent.hpp>
#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/game/ctf.hpp>
#include <c_lib/items/items.hpp>

namespace ServerState
{
    Agent_list agent_list;
    Cspray_list cspray_list;
    Grenade_list grenade_list;
    Neutron_list neutron_list;
    Monsters::Slime_list slime_list;
    Voxel_hitscan_list voxel_hitscan_list;
    Spawner_list spawner_list;

    CTF ctf;

    void init()
    {
        ctf.init();
    }

    void damage_objects_within_sphere(
        float x, float y, float z, float radius,
        int dmg, int owner, Object_types inflictor_type
    )
    {
        Agent_state* agent = agent_list.get(owner);
        // dont check null here, its not required.
        // agent could have disconnected, we'll let his grenades stay effective
        int i;                
        int coins = 0;

        // agents
        agent_list.objects_within_sphere(x,y,z,radius);
        Agent_state* a;
        for (i=0; i<agent_list.n_filtered; i++)
        {
            a = agent_list.filtered_objects[i];
            if (a == NULL) continue;
            a->status.apply_damage(dmg, owner, inflictor_type); // need to be able to pass owner & suicidal arguments to apply_damage
        }

        // slimes
        slime_list.objects_within_sphere(x,y,z,radius);
        Monsters::Slime* slime;
        for (i=0; i<slime_list.n_filtered; i++)
        {
            slime = slime_list.filtered_objects[i];
            if (slime == NULL) continue;
            slime_list.destroy(slime->id);
        }

        // spawners
        spawner_list.objects_within_sphere(x,y,z,radius);
        Spawner* s;
        for (i=0; i<spawner_list.n_filtered; i++)
        {
            s = spawner_list.filtered_objects[i];
            if (s==NULL) continue;
            int h = s->take_damage(dmg);
            if (h <= 0 && agent != NULL)
                coins += s->get_coins_for_kill(agent->status.team);
        }

        // turrets
        //turrent_list.objects_within_sphere(x,y,z,radius);
        //Turrent* t;
        //for (i=0; i<turrent_list.n_filtered; i++)
        //{
            //t = turrent_list.filtered[i];
            //if (t==NULL) continue;
            //t->take_damage(dmg);
            //if (h <= 0 && agent != NULL)
                //coins += s->get_coins_for_kill(agent->status.team);
        //}

    // add all the coins
    if (agent != NULL)
        agent->status.add_coins(coins);

    }

    void server_tick()
    {
        spawner_list.tick();
        ctf.tick();
    }

    void send_game_state_to_client(int client_id)
    {
        // agents
        // mobs
        // "game" (ctf)
        // items

        agent_list.send_to_client(client_id);
        slime_list.send_to_client(client_id);
        ctf.send_to_client(client_id);
        spawner_list.send_to_client(client_id);
    }

    void add_player_to_chat(int client_id)
    {
        chat_server.player_join(client_id);
    }


}
#endif
