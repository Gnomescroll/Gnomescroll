#include "server_state.hpp"

#include <c_lib/agent/agent.hpp>

#include <c_lib/game/ctf.hpp>
#include <c_lib/items/items.hpp>

#include <c_lib/chat/interface.hpp>

namespace ServerState
{
    Agent_list* agent_list = NULL;


    Voxel_hitscan_list* voxel_hitscan_list = NULL;
    SpawnerList* spawner_list = NULL;
    OwnedList* owned_list = NULL;
    
    //Grenade_shrapnel_list* grenade_shrapnel_list;

    GameObject_list* object_list;

    CTF* ctf = NULL;

    void init_lists()
    {
        voxel_hitscan_list = new Voxel_hitscan_list;
        spawner_list = new SpawnerList; // functions similar to Voxel_hitscan_list; objects must register with it
        owned_list = new OwnedList;
        
        //neutron_list = new Neutron_list;
        //grenade_shrapnel_list = new Grenade_shrapnel_list;

        agent_list = new Agent_list;
        object_list = new GameObject_list;
    }

    void teardown_lists()
    {
        // voxels
        delete agent_list;
        delete object_list;

        delete voxel_hitscan_list; // must go last
        delete spawner_list;
        delete owned_list;
    }

    //move this into interface
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
    
    //move this into interface
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
        init_lists();
        init_ctf();
    }

    void teardown()
    {
        teardown_ctf();
        teardown_lists();
    }

    void damage_objects_within_sphere(
        float x, float y, float z, float radius,
        int dmg, int owner,
        Object_types inflictor_type, int inflictor_id,
        bool suicidal   // defaults to true; if not suicidal, agent's with id==owner will be skipped
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
            if (!suicidal && a->id == owner) continue;
            if (!a->point_can_cast(x, y, z, radius)) continue;  // cheap terrain cover check
            dmg *= gaussian_value(blast_mean, blast_stddev, agent_list->filtered_object_distances[i] / radius);
            a->status.apply_damage(dmg, owner, inflictor_type);
        }

        if (agent == NULL) return; // return here; turrets/spawners are team items and we need to know the agent's team

        // Spawners, Turrets etc
        const int filter_n_types = 3;
        const Object_types filter_types[filter_n_types] = { OBJ_TYPE_TURRET, OBJ_TYPE_SPAWNER, OBJ_TYPE_SLIME };
        object_list->objects_within_sphere(filter_types, filter_n_types, x,y,z, radius);
        ObjectPolicyInterface* obj;
        ObjectState* state;
        for (int i=0; i<object_list->n_filtered; i++)
        {
            obj = object_list->filtered_objects[i];
            if (obj == NULL) continue;
            state = obj->state();

            /* TODO */
            // state->can_be_killed_by(type, id, team)
            // apply teammate rules etc
            if ((obj->get_team() == agent->status.team && obj->get_owner() != NO_AGENT)
              && obj->get_owner() != agent->id)
                continue;

            obj->take_damage(Particles::get_grenade_damage(state->type));
            if (obj->did_die() && agent != NULL
              && !(state->type == inflictor_type && state->id == inflictor_id)) // obj is not self
                coins += get_kill_reward(obj, agent->id, agent->status.team);
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

        agent_list->update_models(); // sets skeleton
        object_list->tick();
        Particles::grenade_list->tick();

        t_item::tick();

        if(counter % 10 == 0)
        {
            t_item::check_item_pickups();
        }

        ctf->tick();
    }

    void send_initial_game_state_to_client(int client_id)
    {
        agent_list->send_to_client(client_id);
        ctf->send_to_client(client_id);

        object_list->send_to_client(OBJ_TYPE_TURRET, client_id);
        object_list->send_to_client(OBJ_TYPE_SPAWNER, client_id);
        object_list->send_to_client(OBJ_TYPE_SLIME, client_id);
    }

    void send_remainining_game_state_to_client(int client_id)
    {
        object_list->send_to_client(OBJ_TYPE_INVENTORY, client_id);
    }

    //move into agent interface
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
        revoke_ownership(agent_id);
    }

    void revoke_ownership(int agent_id)
    {
        owned_list->transfer_ownership(agent_id, NO_AGENT);
    }

}