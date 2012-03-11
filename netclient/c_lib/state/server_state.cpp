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
    Agent_list* agent_list = NULL;
    Cspray_list* cspray_list = NULL;
    Grenade_list* grenade_list = NULL;
    //Neutron_list* neutron_list = NULL;
    Monsters::Slime_list* slime_list = NULL;
    Voxel_hitscan_list* voxel_hitscan_list = NULL;
    Spawner_list* spawner_list = NULL;
    Turret_list* turret_list = NULL;

    CTF* ctf = NULL;

    void init_lists()
    {
        agent_list = new Agent_list;
        cspray_list = new Cspray_list;
        grenade_list = new Grenade_list;
        //neutron_list = new Neutron_list;
        slime_list = new Monsters::Slime_list;
        voxel_hitscan_list = new Voxel_hitscan_list;
        spawner_list = new Spawner_list;
        turret_list = new Turret_list;
    }

    void teardown_lists()
    {
        delete cspray_list;
        delete grenade_list;
        //delete neutron_list;

        // voxels
        delete agent_list;
        delete slime_list;
        delete spawner_list;
        delete turret_list;
        delete voxel_hitscan_list; // must go last
    }

    static void init_ctf()
    {
        static int inited = 0;
        if (inited++)
        {
            printf("WARNING: ServerState::init_ctf -- attempt to call more than once\n");
            return;
        }
        ctf = new CTF;
        ctf->init();
    }
    
    static void teardown_ctf()
    {
        if (ctf != NULL)
            delete ctf;
    }

    void init()
    {
        static int inited = 0;
        if (inited++)
        {
            printf("WARNING: ServerState::init -- attempt to call more than once\n");
            return;
        }
        init_ctf();
    }

    void teardown()
    {
        teardown_ctf();
    }

    void damage_objects_within_sphere(
        float x, float y, float z, float radius,
        int dmg, int owner, Object_types inflictor_type
    )
    {
        Agent_state* agent = agent_list->get(owner);
        // dont check null here, its not required.
        // agent could have disconnected, we'll let his grenades stay effective
        int i;                
        int coins = 0;

        // agents
        agent_list->objects_within_sphere(x,y,z,radius);
        Agent_state* a;
        const float blast_mean = 0;
        const float blast_stddev = 0.5;
        for (i=0; i<agent_list->n_filtered; i++)
        {
            a = agent_list->filtered_objects[i];
            if (a == NULL) continue;
            if (!a->point_can_cast(x, y, z, radius)) continue;  // cheap terrain cover check
            dmg *= gaussian_value(blast_mean, blast_stddev, agent_list->filtered_object_distances[i] / radius);
            a->status.apply_damage(dmg, owner, inflictor_type);
        }

        // slimes
        slime_list->objects_within_sphere(x,y,z,radius);
        Monsters::Slime* slime;
        for (i=0; i<slime_list->n_filtered; i++)
        {
            slime = slime_list->filtered_objects[i];
            if (slime == NULL) continue;
            slime_list->destroy(slime->id);
        }

        // spawners
        spawner_list->objects_within_sphere(x,y,z,radius);
        Spawner* s;
        for (i=0; i<spawner_list->n_filtered; i++)
        {
            s = spawner_list->filtered_objects[i];
            if (s==NULL) continue;
            if ((s->get_team() == agent->status.team && s->owner != NO_AGENT_OWNER)
            && s->owner != agent->id)
                continue; // teammates cant kill grenades
            int h = s->take_damage(GRENADE_SPAWNER_DAMAGE);
            if (h <= 0 && agent != NULL)
                coins += s->get_coins_for_kill(agent->id, agent->status.team);
        }

        // turrets
        turret_list->objects_within_sphere(x,y,z,radius);
        Turret* t;
        for (i=0; i<turret_list->n_filtered; i++)
        {
            t = turret_list->filtered_objects[i];
            if (t==NULL) continue;
            if ((t->get_team() == agent->status.team && t->owner != NO_AGENT_OWNER)
            && t->owner != agent->id)
                continue; // teammates cant kill turrets
            int h = t->take_damage(GRENADE_TURRET_DAMAGE);
            if (h <= 0 && agent != NULL)
                coins += t->get_coins_for_kill(agent->id, agent->status.team);
        }

        // add all the coins
        if (agent != NULL)
            agent->status.add_coins(coins);

    }

/*
    Map update function called from here

    4 times per second per client.  Maybe too fast?
*/
    void server_tick()
    {   
        static int counter = 0;
        counter++;
    
        t_map::t_map_send_map_chunks();  //every tick

        if(counter % 15 == 0) 
        {
            agent_list->update_map_manager_positions();
            t_map::t_map_manager_update();
            t_map::t_map_sort_map_chunk_ques();
        }

/*
        if(counter % 2 == 0) 
        {
            agent_list->update_map_manager_positions();
            t_map::t_map_manager_update();
            t_map::t_map_sort_map_chunk_ques();
            t_map::t_map_send_map_chunks();
        }
*/
        agent_list->update_models(); // sets skeleton
        spawner_list->tick();
        turret_list->tick();
        ctf->tick();
    }

    void send_game_state_to_client(int client_id)
    {
        agent_list->send_to_client(client_id);
        slime_list->send_to_client(client_id);
        ctf->send_to_client(client_id);
        spawner_list->send_to_client(client_id);
        turret_list->send_to_client(client_id);
    }

    void send_id_to_client(int client_id)
    {
        PlayerAgent_id_StoC msg;
        msg.id = client_id;
        msg.sendToClient(client_id);
    }

    void add_player_to_chat(int client_id)
    {
        chat_server->player_join(client_id);
    }

    void remove_player_from_chat(int client_id)
    {
        Agent_state* a = agent_list->get(client_id);
        if (a==NULL) {
            printf("WARNING ServerState::remove_player_from_chat -- id %d does not have an agent\n", client_id);
            return;
        }
        chat_server->player_quit(client_id, a->status.team);
    }

    char* agent_name(int id)
    {
        Agent_state* a = ServerState::agent_list->get(id);
        if (a==NULL)
            return NULL;
        if (!a->status.identified)
            return NULL;
        return a->status.name;
    }

    void send_disconnect_notice(int client_id)
    {
        Agent_state* a = NetServer::agents[client_id];
        char* name;
        if (a == NULL)
        {
            printf("agent was already destroyed\n");
            name = (char*)"";
        }
        else
            name = a->status.name;
        client_disconnected_StoC msg;
        msg.id = client_id;
        strcpy(msg.name, name);
        msg.broadcast();
    }

    void start_game()
    {
        int address[4];
        address_from_string(Options::ip_address, address);
        init_net_server(address[0], address[1], address[2], address[3], Options::port);
        ctf->start();
    }

    void agent_disconnect(int agent_id)
    {
        remove_player_from_chat(agent_id);
        ctf->remove_agent_from_team(agent_id);
        send_disconnect_notice(agent_id);
        agent_list->destroy(agent_id);
        turret_list->alter_owner(agent_id, NO_AGENT_OWNER);
        spawner_list->alter_owner(agent_id, NO_AGENT_OWNER);
    }

    void send_version_to_client(int client_id)
    {
        version_StoC msg;
        msg.version = DC_VERSION;
        msg.sendToClient(client_id);
    }

}
#endif
