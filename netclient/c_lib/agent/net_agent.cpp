#include "net_agent.hpp"

#include <c_lib/agent/agent.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/defines.h>
#include <c_lib/weapons/weapons.hpp>
#include <monsters/monsters.hpp>

#ifdef DC_CLIENT
#include <c_lib/time/physics_timer.h>
#include <c_lib/chat/client.hpp>
#endif

#ifdef DC_SERVER
#include <c_lib/t_map/t_map.hpp>
#include <c_lib/ray_trace/ray_trace.hpp>
#endif


/*
 *  Player Agent Packets
 */

inline void PlayerAgent_Snapshot::handle()
{
    #ifdef DC_CLIENT
    ClientState::playerAgent_state.handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
    #endif
    //printf("Received Agent_state_message packet: agent_id= %i \n", id);
    //printf("seq= %i \n", seq);
    return;
}

// Server -> Client handlers
#ifdef DC_CLIENT

inline void SendClientId_StoC::handle()
{
    NetClient::Server.client_id = client_id;

    if (ClientState::playerAgent_state.you != NULL)
        ClientState::playerAgent_state.you->client_id = client_id;

    ClientState::client_id_received(client_id);
}


inline void Agent_state_message::handle()
{
    Agent_state* a = STATE::agent_list->get(id);
    if(a == NULL)
    {
        //printf("Agent_state_message -- Agent %d does not exist\n", id);
        return;
    }
    a->handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
}

inline void Agent_teleport_message::handle()
{
    Agent_state* a = STATE::agent_list->get(id);
    if(a == NULL)
    {
        //printf("Agent_teleport_message -- Agent %d does not exist\n", id);
        return;
    }
    if (a->status.team == 0) return;
    // reset camera angle
    if (a->is_you() && agent_camera != NULL)
        agent_camera->set_angles(theta, phi);
    a->set_state(x,y,z, vx,vy,vz);
    a->set_angles(theta, phi);
}

//Agent control state, server to client
inline void Agent_cs_StoC::handle()
{
    Agent_state* a = STATE::agent_list->get(id);
    if(a == NULL)
    {
        //printf("Agent_control_to_client_message: agent does not exist, id= %i\n", id);
        return;
    }
    //printf("!!! control state= %i \n", cs);
    a->handle_control_state(seq, cs, theta, phi);
    ClientState::playerAgent_state.handle_net_control_state(seq, cs, theta, phi);
}

// damage indicator packet
inline void agent_damage_StoC::handle()
{
    Agent_state* a = STATE::agent_list->get(id);
    if(a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.took_damage(dmg);
}

inline void agent_shot_object_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("WARNING: agent_shot_object_StoC -- Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.fired_weapon_at_object(target_id, target_type, target_part);
    // get obj from metadata, set voxel
    int voxel[3];
    voxel[0] = vx;
    voxel[1] = vy;
    voxel[2] = vz;
    int voxel_blast_radius = 1;
    if (target_type == OBJ_TYPE_AGENT)
    {
        Agent_state* target = ClientState::agent_list->get(target_id);
        if (target->status.team == a->status.team) return;
        voxel_blast_radius = 3;
    }
    else if (target_type == OBJ_TYPE_SLIME)
    {
        voxel_blast_radius = 2;
    }
    destroy_object_voxel(target_id, target_type, target_part, voxel, voxel_blast_radius);
}

inline void agent_shot_block_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.fired_weapon_at_block(x,y,z, cube, side);
}

inline void agent_shot_nothing_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.fired_weapon_at_nothing();
}

inline void agent_melee_object_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.melee_attack_object(target_id, target_type, target_part);
    // get obj from metadata, set voxel
    int voxel[3];
    voxel[0] = vx;
    voxel[1] = vy;
    voxel[2] = vz;
    int voxel_blast_radius = 1;
    if (target_type == OBJ_TYPE_AGENT)
    {
        Agent_state* target = ClientState::agent_list->get(target_id);
        if (target->status.team == a->status.team) return;
        voxel_blast_radius = 3;
    }
    else if (target_type == OBJ_TYPE_SLIME)
    {
        voxel_blast_radius = 2;
    }
    destroy_object_voxel(target_id, target_type, target_part, voxel, voxel_blast_radius);
}

inline void agent_melee_nothing_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.melee_attack_nothing();
}

inline void agent_hit_block_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.hit_block();
}

inline void agent_threw_grenade_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.threw_grenade();
}

inline void agent_placed_block_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.placed_block();
}

inline void agent_dead_StoC::handle()
{
    bool _dead = (bool)dead;
    Agent_state* a = ClientState::agent_list->get(id);
    if(a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.life_changing(_dead);
}

inline void agent_health_StoC::handle()
{
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.healed(health);
}

inline void agent_create_StoC::handle()
{
    Agent_state* a = ClientState::agent_list->get_or_create(id);
    if (a == NULL)
    {
        printf("agent_create_StoC:: get_or_create agent %d failed\n", id);
        return;
    }
    a->client_id = client_id;
    a->event.joined_team(team);
    //printf("C Agent created. id: %d\n", a->id);
}

inline void agent_name_StoC::handle()
{
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        //printf("agent_name_StoC:: agent %d unknown. Could not name %s\n", id, name);
        return;
    }
    char* old_name = (char*)calloc(strlen(a->status.name) + 1, sizeof(char));
    strcpy(old_name, a->status.name);
    bool new_name = a->status.set_name(name);
    if (new_name)
        a->event.name_changed(old_name);
    free(old_name);
}

inline void agent_destroy_StoC::handle()
{
    printf("destroying agent %d\n", id);
    ClientState::agent_list->destroy(id);
}

inline void PlayerAgent_id_StoC::handle()
{
    ClientState::set_PlayerAgent_id(id);
}

inline void AgentKills_StoC::handle()
{
    Agent_state* a = ClientState::agent_list->get(id);
    if(a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->status.kills = kills;
}

inline void AgentDeaths_StoC::handle()
{
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->status.deaths = deaths;
}

inline void AgentSuicides_StoC::handle()
{
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->status.suicides = suicides;
}

inline void AgentActiveWeapon_StoC::handle()
{
    Agent_state* a =  ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    //a->weapons.set_active(slot);  // dont use! will end up in recursive packet chain
    a->weapons.active = slot;
}

inline void AgentReloadWeapon_StoC::handle()
{
    Agent_state* a = ClientState::agent_list->get(id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }

    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->event.reload_weapon(type);
}

inline void agent_coins_StoC::handle()
{
    Agent_state* a = ClientState::playerAgent_state.you;
    if (a == NULL) return;
    a->event.coins_changed(coins);
}

inline void identified_StoC::handle()
{
    Agent_state* a = ClientState::playerAgent_state.you;
    if (a == NULL)
    {
        printf("identified_StoC -- identified as %s but player agent not assigned\n", name);
        return;
    }
    ClientState::playerAgent_state.identified = true;
    char* old_name = (char*)calloc(strlen(a->status.name) + 1, sizeof(char));
    strcpy(old_name, a->status.name);
    bool new_name = a->status.set_name(name);
    if (new_name)
        a->event.name_changed(old_name);
    free(old_name);
}

inline void ping_StoC::handle()
{
    ClientState::last_ping_time = _GET_MS_TIME() - ticks;
}

inline void ping_reliable_StoC::handle()
{
    ClientState::last_reliable_ping_time = _GET_MS_TIME() - ticks;
}

inline void agent_conflict_notification_StoC::handle()
{
    bool suicide = (victim == attacker) ? true : false;

    Agent_state* a = ClientState::agent_list->get(attacker);
    Agent_state* b = ClientState::agent_list->get(victim);
    
    char unknown_name[] = "Someone";
    char *a_name = (a == NULL) ? unknown_name : a->status.name;
    char *b_name = (b == NULL) ? unknown_name : b->status.name;

    char* msg = (char*)calloc(150, sizeof(char));
    switch (method)
    {
        case DEATH_NORMAL:
            if (suicide)
            {
                if (a != NULL && a->is_you())
                    strcpy(msg, "You killed yourself");
                else
                    sprintf(msg, "%s killed themself", a_name);
            }
            else
            {
                if (a != NULL && a->is_you())
                    sprintf(msg, "You killed %s", b_name);
                else if (b != NULL && b->is_you())
                    sprintf(msg, "You were killed by %s", a_name);
                else
                    sprintf(msg, "%s killed %s", a_name, b_name);
            }
            break;

        case DEATH_HEADSHOT:
            if (suicide)
            {
                if (a != NULL && a->is_you())
                    strcpy(msg, "You shot yourself in the head");
                else
                    sprintf(msg, "%s shot themself in the head", a_name);
            }
            else
            {
                if (a != NULL && a->is_you())
                    sprintf(msg, "You shot %s in the head", b_name);
                else if (b != NULL && b->is_you())
                    sprintf(msg, "You shot in the head by %s", a_name);
                else
                    sprintf(msg, "%s shot %s in the head", a_name, b_name);
            }
            break;

        case DEATH_GRENADE:
            if (suicide)
            {
                if (a != NULL && a->is_you())
                    strcpy(msg, "You blew yourself up");
                else
                    sprintf(msg, "%s blew themself up", a_name);
            }
            else
            {
                if (a != NULL && a->is_you())
                    sprintf(msg, "You destroyed %s with a grenade", b_name);
                else if (b != NULL && b->is_you())
                    sprintf(msg, "You were mangled by %s's grenade", a_name);
                else
                    sprintf(msg, "%s ripped %s to pieces", a_name, b_name);
            }
            break;

        case DEATH_BELOW_MAP:
            if (a != NULL && a->is_you())
                strcpy(msg, "You found the afterlife.");
            else
                strcpy(msg, "You hear a faint scream.");
            break;

        case DEATH_FALL:
            if (a != NULL && a->is_you())
                strcpy(msg, "Ouch");
            else
                sprintf(msg, "%s died from a fall.", a_name);
            break;

        case DEATH_TURRET:
            if (b != NULL && b->is_you())
                strcpy(msg, "You were gunned down.");
            else
                sprintf(msg, "%s was destroyed by %s's turret.", b_name, a_name);
            break;
            
        default: break;
    }
    
    chat_client->send_system_message(msg);
    free(msg);
}

inline void version_StoC::handle()
{
    printf("Client Version: %d\n", DC_VERSION);
    printf("Server Version: %d\n", version);
    if (DC_VERSION != version)
    {
        printf("WARNING: Version mismatch\n");
        NetClient::Server.version_match = false;
    }
    else
        NetClient::Server.version_match = true;
}

inline void client_disconnected_StoC::handle()
{
    const char fmt[] = "%s has left the game";
    char* msg = (char*)calloc(strlen(fmt) + strlen(name) - 2 + 1, sizeof(char));
    sprintf(msg, fmt, name);
    chat_client->send_system_message(msg);
    free(msg);
}

inline void spawn_location_StoC::handle()
{
    using ClientState::playerAgent_state;
    if (playerAgent_state.you == NULL) return;
    playerAgent_state.you->event.set_spawner(pt);
}

inline void alter_item_ownership_StoC::handle()
{
    void* obj;
    switch (type)
    {
        case OBJ_TYPE_SPAWNER:
            obj = ClientState::spawner_list->get(id);
            if (obj == NULL) return;
            ((Spawner*)obj)->set_owner(owner);
            break;
        case OBJ_TYPE_TURRET:
            //obj = ClientState::turret_list->get(id);
            obj = ClientState::object_list->get(id);
            if (obj == NULL) return;
            //((Turret*)obj)->set_owner(owner);
            ((ObjectPolicyInterface*)obj)->state()->set_owner(owner);
            break;
        default:
            printf("alter_item_ownership_StoC::handle() -- unhandled item type %d\n", type);
            return;
        
    }
}

inline void destroy_voxel_StoC::handle()
{
    int voxel[3] = { x,y,z };
    destroy_object_voxel(entity_id, entity_type, entity_part, voxel, radius);
    //printf("destroy_voxel_StoC -- %d,%d,%d :: %d,%d,%d :: %d\n", entity_id, entity_type, entity_part, x,y,z, radius);
}

inline void Agent_cs_CtoS::handle() {}
inline void hit_block_CtoS::handle() {}
inline void hitscan_object_CtoS::handle() {}
inline void hitscan_block_CtoS::handle() {}
inline void hitscan_none_CtoS::handle() {}
inline void ThrowGrenade_CtoS::handle(){}
inline void AgentActiveWeapon_CtoS::handle() {}
inline void AgentReloadWeapon_CtoS::handle(){}
inline void agent_set_block_CtoS::handle() {}
inline void place_spawner_CtoS::handle(){}
inline void place_turret_CtoS::handle(){}
inline void melee_object_CtoS::handle(){}
inline void melee_none_CtoS::handle(){}
inline void identify_CtoS::handle(){}
inline void ping_CtoS::handle(){}
inline void ping_reliable_CtoS::handle(){}
inline void choose_spawn_location_CtoS::handle(){}
inline void request_agent_name_CtoS::handle(){}
#endif

// Client -> Server handlers
#ifdef DC_SERVER

inline void SendClientId_StoC::handle() {}
inline void AgentKills_StoC::handle() {}
inline void AgentDeaths_StoC::handle() {}
inline void AgentSuicides_StoC::handle() {}
inline void Agent_state_message::handle() {}
inline void Agent_teleport_message::handle() {}
inline void Agent_cs_StoC::handle() {}
inline void agent_damage_StoC::handle() {}
inline void agent_shot_object_StoC::handle(){}
inline void agent_shot_block_StoC::handle(){}
inline void agent_shot_nothing_StoC::handle(){}
inline void agent_melee_object_StoC::handle(){}
inline void agent_melee_nothing_StoC::handle(){}
inline void agent_hit_block_StoC::handle(){}
inline void agent_threw_grenade_StoC::handle(){}
inline void agent_placed_block_StoC::handle(){}
inline void agent_health_StoC::handle() {}
inline void agent_dead_StoC::handle() {}
inline void agent_create_StoC::handle() {}
inline void agent_destroy_StoC::handle() {}
inline void PlayerAgent_id_StoC::handle() {}
inline void AgentActiveWeapon_StoC::handle() {}
inline void AgentReloadWeapon_StoC::handle() {}
inline void agent_name_StoC::handle() {}
inline void agent_coins_StoC::handle() {}
inline void identified_StoC::handle(){}
inline void ping_StoC::handle(){}
inline void ping_reliable_StoC::handle(){}
inline void agent_conflict_notification_StoC::handle(){}
inline void version_StoC::handle(){}
inline void client_disconnected_StoC::handle(){}
inline void spawn_location_StoC::handle(){}
inline void alter_item_ownership_StoC::handle(){}
inline void destroy_voxel_StoC::handle(){}

//for benchmarking
//static int _total = 0;
//static const int a_DEBUG = 1;

inline void Agent_cs_CtoS::handle()
{
    //printf("cs_CtoS: seq= %i \n", seq);

    //for benchmarking
    /*
    if(a_DEBUG)
    {
        _total++;
        if(_total % 1000 == 0) printf("%i messages\n", _total);
        return;
    }
    */

    Agent_state* A = NetServer::agents[client_id];
    if(A == NULL) return;

    //determine if message is new, if so send out
    /*
        Client should send last 2 control states each packet, must handle redundant control state properly
    */
    Agent_cs_StoC M;
    M.id = A->id;
    M.seq = seq;
    M.cs = cs;
    M.theta = theta;
    M.phi = phi;
    M.broadcast();

    A->handle_control_state(seq, cs, theta, phi);

    /*
        Warning: setting agent client id by the client the last control state was received for that agent
        This needs to be done properly, at agent creation
    */
    //A->client_id = client_id;

}

// agent hit block action
inline void hit_block_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    if (!a->weapons.pick.fire()) return;
    agent_hit_block_StoC msg;
    msg.id = a->id;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.broadcast();
    
    //int dmg = a->weapons.pick.damage;
    int dmg = 32;
    t_map::apply_damage_broadcast(x,y,z, dmg, t_map::TMA_PICK);
}

inline void hitscan_object_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    if (!a->weapons.laser.fire()) return;

    //const int agent_dmg = 25;
    const int slime_dmg = 25;
    const int spawner_dmg = 25;
    const int turret_dmg = 25;
    int dmg_health;

    int voxel[3] = { vx,vy,vz };
    Agent_state* agent = NULL;
    Monsters::Slime* slime = NULL;
    Spawner* spawner = NULL;
    Turret* turret = NULL;

    switch (type)
    {
        case OBJ_TYPE_AGENT:
            agent = ServerState::agent_list->get(id);
            if (agent == NULL || agent->vox == NULL) return;
            agent->vox->update(a->s.x, a->s.y, a->s.z, a->s.theta, a->s.phi);
            // apply damage
            agent->status.apply_hitscan_laser_damage_to_part(part, a->id, a->type);
            //x = agent->s.x;
            //y = agent->s.y;
            //z = agent->s.z;
            destroy_object_voxel(agent->id, agent->type, part, voxel, 3);     
            break;

        case OBJ_TYPE_SLIME:
            slime = ServerState::slime_list->get(id);
            if (slime==NULL) return;
            // apply damage
            dmg_health = slime->take_damage(slime_dmg);
            if (dmg_health <= 0)
                a->status.kill_slime();
            //x = slime->x;
            //y = slime->y;
            //z = slime->z;
            break;

        case OBJ_TYPE_SPAWNER:
            spawner = ServerState::spawner_list->get(id);
            if (spawner == NULL) return;

            if ((spawner->get_team() == a->status.team && spawner->get_owner() != NO_AGENT)
              && spawner->get_owner() != a->id)
                return; // teammates cant kill spawners
                
            // apply damage
            dmg_health = spawner->take_damage(spawner_dmg);
            if (dmg_health <= 0)
            {   // TODO -- add coins inside the take_damage() method
                int coins = spawner->get_coins_for_kill(a->id, a->status.team);
                a->status.add_coins(coins);
            }
            //x = spawner->x;
            //y = spawner->y;
            //z = spawner->z;
            break;

        case OBJ_TYPE_TURRET:
            //turret = ServerState::turret_list->get(id);
            turret = (Turret*)ServerState::object_list->get(id);
            if (turret == NULL) return;

            if ((turret->state()->get_team() == a->status.team && turret->state()->get_owner() != NO_AGENT)
              && turret->state()->get_owner() != a->id)
                return; // teammates cant kill turrets
                
            // apply damage
            dmg_health = turret->state()->take_damage(turret_dmg);
            if (dmg_health <= 0)
            {
                //int coins = turret->get_coins_for_kill(a->id, a->status.team);
                int coins = turret->state()->get_kill_reward(a->id, a->status.team);
                a->status.add_coins(coins);
            }
            break;
        default:
            printf("hitscan_object_CtoS::handle -- Unknown object type %d\n", type);
            return;
    }
    agent_shot_object_StoC msg;
    msg.id = a->id;
    msg.target_id = this->id;
    msg.target_type = this->type;
    msg.target_part = this->part;
    msg.vx = vx;
    msg.vy = vy;
    msg.vz = vz;
    msg.broadcast();

}

// hitscan target:block
inline void hitscan_block_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }

    #if !PRODUCTION
    a->status.add_coins(100);
    #endif

    if (a->status.team == 0) return;
    if (!a->weapons.laser.fire()) return;

    // get collision point on block surface (MOVE THIS TO A BETTER SPOT)
    // send to clients
    int collision[3];
    int pre_collision[3];
    int side[3];
    int cube;
    const float max_l = 500.0f;
    float distance=0.0f;

    float f[3];
    a->s.forward_vector(f);

    float
        fx = a->s.x,
        fy = a->s.y,
        fz = a->camera_z();

    int collided = _ray_cast6(fx,fy,fz, f[0], f[1], f[2], max_l, &distance, collision, pre_collision, &cube, side);
    if (!collided) return;
    // pt of collision
    fx += f[0] * distance;
    fy += f[1] * distance;
    fz += f[2] * distance;

    int cube_side = get_cube_side_from_side_array(side);

    agent_shot_block_StoC msg;
    msg.id = a->id;
    msg.x = fx;
    msg.y = fy;
    msg.z = fz;
    msg.cube = cube;
    msg.side = cube_side;
    msg.broadcast();

    // damage block
    // WARNING:
    // *must* call this after raycasting, or you will be raycasting altered terrain
    int weapon_block_damage = 12;
    t_map::apply_damage_broadcast(x,y,z, weapon_block_damage, t_map::TMA_LASER);
    //printf("hitscan block %d:: %d,%d,%d\n", id, x,y,z);
    // TODO: Use weapon block dmg
}

inline void hitscan_none_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    if (!a->weapons.laser.fire()) return;
    agent_shot_nothing_StoC msg;
    msg.id = a->id;
    msg.broadcast();
}

inline void melee_object_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    if (!a->weapons.pick.fire()) return;

    Agent_state* agent = NULL;
    Monsters::Slime* slime = NULL;
    Spawner* spawner = NULL;
    Turret* turret = NULL;

    //const int agent_dmg = 50;
    const int slime_dmg = 50;
    const int spawner_dmg = 50;
    const int turret_dmg = 50;
    int dmg_health;
    int voxel[3] = { vx,vy,vz };
    
    switch (type)
    {
        case OBJ_TYPE_AGENT:
            agent = ServerState::agent_list->get(id);
            if (agent==NULL) return;
            // apply damage
            dmg_health = agent->status.apply_hitscan_laser_damage_to_part(part, a->id, a->type);
            if (dmg_health <= 0)
                a->status.add_coins(1);
            destroy_object_voxel(agent->id, agent->type, part, voxel, 3);
            break;

        case OBJ_TYPE_SLIME:
            slime = ServerState::slime_list->get(id);
            if (slime==NULL) return;
            // apply damage
            dmg_health = slime->take_damage(slime_dmg);
            if (dmg_health <= 0)
                a->status.kill_slime();
            break;

        case OBJ_TYPE_SPAWNER:
            spawner = ServerState::spawner_list->get(id);
            if (spawner == NULL) return;

            if ((spawner->get_team() == a->status.team && spawner->get_owner() != NO_AGENT)
            && spawner->get_owner() != a->id)
                return; // teammates cant kill spawners
                
            // apply damage
            dmg_health = spawner->take_damage(spawner_dmg);
            if (dmg_health <= 0)
            {
                int coins = spawner->get_coins_for_kill(a->id, a->status.team);
                a->status.add_coins(coins);
            }
            break;

        case OBJ_TYPE_TURRET:
            //turret = ServerState::turret_list->get(id);
            turret = (Turret*)ServerState::object_list->get(id);
            if (turret == NULL) return;

            if ((turret->state()->get_team() == a->status.team && turret->state()->get_owner() != NO_AGENT)
            && turret->state()->get_owner() != a->id)
                return; // teammates cant kill turrets
                
            // apply damage
            dmg_health = turret->state()->take_damage(turret_dmg);
            if (dmg_health <= 0)
            {
                //int coins = turret->get_coins_for_kill(a->id, a->status.team);
                int coins = turret->state()->get_kill_reward(a->id, a->status.team);
                a->status.add_coins(coins);
            }
            break;

        default:
            printf("hitscan_object_CtoS::handle -- Unknown object type %d\n", type);
            return;
    }
    agent_melee_object_StoC msg;
    msg.id = a->id;
    msg.target_id = this->id;
    msg.target_type = this->type;
    msg.target_part = this->part;
    msg.vx = vx;
    msg.vy = vy;
    msg.vz = vz;
    msg.broadcast();
}

inline void melee_none_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    if (!a->weapons.pick.fire()) return;
    agent_melee_nothing_StoC msg;
    msg.id = a->id;
    msg.broadcast();
}

inline void ThrowGrenade_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    if (!a->weapons.grenades.fire()) return;
    agent_threw_grenade_StoC msg;
    msg.id = a->id;
    msg.broadcast();

    Vec3 n = vec3_init(vx,vy,vz);
    normalize_vector(&n);
    static const float PLAYER_ARM_FORCE = 15.0f; // load from dat later
    //create grenade
    n = vec3_scalar_mult(n, PLAYER_ARM_FORCE);
    Grenade* g = ServerState::grenade_list->create(x,y,z, n.x, n.y, n.z);
    if (g==NULL) return;
    g->owner = a->id;
    g->broadcast();
}

inline void AgentActiveWeapon_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    a->weapons.set_active(slot);
}

inline void AgentReloadWeapon_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    a->weapons.reload(type);
    // forward action
    AgentReloadWeapon_StoC msg;
    msg.id = a->id;
    msg.type = type;
    msg.broadcast();
}

inline void agent_set_block_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    // fire block applier
    if (a->status.team == 0) return;
    if (!a->weapons.blocks.fire()) return;
    agent_placed_block_StoC msg;
    msg.id = a->id;
    msg.broadcast();
    
    // do block place checks here later
    // problem is, fire/(decrement ammo) packet is separate, and isnt aware of this failure

    // check this player first, most likely to be colliding
    bool collides = false;
    _set(x,y,z, val); // set temporarily to test against
    if (agent_collides_terrain(a))
    {
        collides = true;
        printf("collides w/ player\n");
    }
    else
    {
        for (int i=0; i<ServerState::agent_list->n_max; i++)
        {
            Agent_state* agent = ServerState::agent_list->a[i];
            if (agent == NULL || agent == a) continue;
            if (agent_collides_terrain(agent))
            {
                collides = true;
                break;
            }
        }
    }
    _set(x,y,z,0);  // unset

    if (!collides)
        _set_broadcast(x,y,z, val);    
}

#define ITEM_PLACEMENT_Z_DIFF_LIMIT 3
inline void place_spawner_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    if (!a->status.can_purchase(OBJ_TYPE_SPAWNER)) return;
    if (ServerState::spawner_list->full()) return;
    if (!ServerState::spawner_list->team_spawner_available(a->status.team)) return;
    if (ServerState::spawner_list->point_occupied((int)x, (int)y, (int)z)) return;
    // zip down
    int new_z = t_map::get_highest_open_block(x,y);
    if (z - new_z > ITEM_PLACEMENT_Z_DIFF_LIMIT || z - new_z < 0) return;
    if (ServerState::spawner_list->point_occupied((int)x, (int)y, (int)new_z)) return;

    Spawner* s = ServerState::spawner_list->create(x+0.5f,y+0.5f,new_z);
    if (s==NULL) return;
    a->status.purchase(OBJ_TYPE_SPAWNER);
    s->set_team(a->status.team);
    s->set_owner(a->id);
    s->init_vox();
    object_create_owner_team_index_StoC msg;
    s->create_message(&msg);
    msg.broadcast();
}

inline void place_turret_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("place_turret_CtoS:: Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    if (!a->status.can_purchase(OBJ_TYPE_TURRET)) return;
    //if (ServerState::turret_list->full()) return; // TODO
    //if (ServerState::turret_list->point_occupied((int)x, (int)y, (int)z)) return; // TODO
    // zip down
    int new_z = t_map::get_highest_open_block(x,y);
    if (z - new_z > ITEM_PLACEMENT_Z_DIFF_LIMIT || z - new_z < 0) return;
    //if (ServerState::turret_list->point_occupied((int)x, (int)y, (int)new_z)) return; // TODO

    a->status.purchase(OBJ_TYPE_TURRET);
    //Turret* t = ServerState::turret_list->create(x+0.5f,y+0.5f,new_z);
    Turret* t = (Turret*)ServerState::object_list->create(x+0.5f,y+0.5f,new_z, 0,0,0, OBJ_TYPE_TURRET);
    if (t==NULL) return;
    t->state()->set_team(a->status.team);    // TODO -- set properties before born() is called
    t->state()->set_owner(a->id);
    t->born();
    //t->init_vox();
    //object_create_owner_team_StoC msg;
    //t->create_message(&msg);
    //msg.broadcast();
}
#undef ITEM_PLACEMENT_Z_DIFF_LIMIT

inline void choose_spawn_location_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    a->status.set_spawner(id);
}

const char DEFAULT_PLAYER_NAME[] = "Clunker";

void adjust_name(char* name, unsigned int len)
{
    if (len >= (int)(PLAYER_NAME_MAX_LENGTH - 4))
    {
        name[PLAYER_NAME_MAX_LENGTH-4-1] = '\0';
    }
    sprintf(name, "%s%04d", name, randrange(0,9999));
}

inline void identify_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("identify_CtoS : handle -- client_id %d has no agent. could not identify\n", client_id);
        return;
    }
    printf("Rceived name %s\n", name);

    unsigned int len = sanitize_player_name(name);
    
    if (len == 0)
        strcpy(name, DEFAULT_PLAYER_NAME);

    if (len >= PLAYER_NAME_MAX_LENGTH)
        name[PLAYER_NAME_MAX_LENGTH-1] = '\0';

    while (!ServerState::agent_list->name_available(name))
        adjust_name(name, len);

    a->status.set_name(name);
    a->status.identified = true;

    identified_StoC msg;
    strcpy(msg.name, name);
    msg.sendToClient(client_id);

    NetServer::clients[client_id]->ready();
}

inline void ping_CtoS::handle()
{
    ping_StoC msg;
    msg.ticks = ticks;
    msg.sendToClient(client_id);
}

inline void ping_reliable_CtoS::handle()
{
    ping_reliable_StoC msg;
    msg.ticks = ticks;
    msg.sendToClient(client_id);
}

inline void request_agent_name_CtoS::handle()
{
    Agent_state* a = ServerState::agent_list->get(id);
    if (a == NULL)
    {
        printf("request_agent_name_CtoS:: unknown agent %d\n", id);
        return;
    }
    
    agent_name_StoC msg;
    strcpy(msg.name, a->status.name);
    msg.id = id;
    msg.sendToClient(client_id);
}
#endif
