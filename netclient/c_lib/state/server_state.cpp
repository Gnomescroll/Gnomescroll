#include "server_state.hpp"

#include <agent/agent.hpp>

#include <game/ctf.hpp>

#include <chat/interface.hpp>

#include <entity/objects.hpp>
#include <entity/components.hpp>

namespace ServerState
{
    Agent_list* agent_list = NULL;


    Voxel_hitscan_list* voxel_hitscan_list = NULL;
    //Grenade_shrapnel_list* grenade_shrapnel_list;

    CTF* ctf = NULL;

/*
    Map update function called from here

    4 times per second per client.  Maybe too fast?
*/
    
    void init_lists()
    {
        voxel_hitscan_list = new Voxel_hitscan_list;
        agent_list = new Agent_list;
        //grenade_shrapnel_list = new Grenade_shrapnel_list;
    }

    void teardown_lists()
    {
        //delete grenade_shrapnel_list;;
        delete agent_list;
        delete voxel_hitscan_list; // must go last
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
        if (ctf != NULL) delete ctf;
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
        int damage, int owner,
        ObjectType inflictor_type, int inflictor_id,
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
            damage *= gaussian_value(blast_mean, blast_stddev, agent_list->filtered_object_distances[i] / radius);
            a->status.apply_damage(damage, owner, inflictor_type);
        }

        if (agent == NULL) return; // return here; turrets/spawners are team items and we need to know the agent's team

        // add all the coins
        if (agent != NULL)
            agent->status.add_coins(coins);

        Vec3 position = vec3_init(x,y,z);

        const int n_team_types = 2;
        const ObjectType team_types[n_team_types] = {
            OBJECT_TURRET, OBJECT_AGENT_SPAWNER,
        };
        Objects::damage_team_objects_within_sphere(team_types, n_team_types, position, radius, damage, agent->status.team, agent->id);

        const int n_types = 3;
        const ObjectType types[n_types] = {
            OBJECT_MONSTER_BOMB, OBJECT_MONSTER_BOX, OBJECT_MONSTER_SPAWNER,
        };
        Objects::damage_objects_within_sphere(types, n_types, position, radius, damage);

    }
        
    void send_initial_game_state_to_client(int client_id)
    {
        agent_list->send_to_client(client_id);
        ctf->send_to_client(client_id);

        Objects::send_to_client(OBJECT_TURRET, client_id);
        Objects::send_to_client(OBJECT_AGENT_SPAWNER, client_id);
        Objects::send_to_client(OBJECT_MONSTER_BOMB, client_id);
        Objects::send_to_client(OBJECT_MONSTER_BOX, client_id);
        Objects::send_to_client(OBJECT_MONSTER_SPAWNER, client_id);
    }

    void send_remainining_game_state_to_client(int client_id)
    {
        // inventory is not in entity system, so these have no effect
        Objects::send_to_client(OBJECT_AGENT_INVENTORY, client_id);
        Objects::send_to_client(OBJECT_AGENT_TOOLBELT, client_id);
        Objects::send_to_client(OBJECT_NANITE_INVENTORY, client_id);
        Objects::send_to_client(OBJECT_CRAFTING_BENCH, client_id);

        Agent_state* agent = agent_list->get(client_id);
        if (agent == NULL) return;
        agent->status.send_inventories_to_client();
    }

    //move somewhere
    char* agent_name(int id)
    {
        Agent_state* a = ServerState::agent_list->get(id);
        if (a==NULL)
            return NULL;
        if (!a->status.identified)
            return NULL;
        return a->status.name;
    }

    //move somewhere
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

    // TODO -- move this test/convenince method
    void spawn_monsters(ObjectType type, int n)
    {
        for (int i=0; i<n; i++)
        {
            Objects::Object* obj = Objects::create(type);
            if (obj == NULL) break;

            Vec3 position;
            position.x = randrange(0, map_dim.x-1);
            position.y = randrange(0, map_dim.y-1);
            position.z = t_map::get_highest_open_block(position.x, position.y);

            using Components::PhysicsComponent;
            PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
            if (physics == NULL)
            {
                Objects::ready(obj);    // sets id
                Objects::destroy(obj);
                break;
            }
            physics->set_position(position);
            Objects::ready(obj);
        }
    }

    void start_game()
    {
        ctf->start();

        // TESTING -- remove later
        // creates mob spawners
        spawn_monsters(OBJECT_MONSTER_BOMB, 100);
        //spawn_monsters(OBJECT_MONSTER_SPAWNER, 12);
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
        printf("revoking ownership\n");
        Components::owner_component_list->revoke(agent_id);
    }

}
