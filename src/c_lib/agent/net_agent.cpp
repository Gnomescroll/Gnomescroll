#include "net_agent.hpp"

#include <agent/agent.hpp>
#if DC_CLIENT
#include <state/client_state.hpp>
#endif
#if DC_SERVER
#include <state/server_state.hpp>
#endif
#include <common/defines.h>

#if DC_CLIENT
#include <common/time/physics_timer.hpp>
#include <chat/client.hpp>
#include <chat/interface.hpp>

#include <SDL/SDL_functions.h>
#endif

#if DC_SERVER
#include <t_map/t_map.hpp>
#include <physics/ray_trace/ray_trace.hpp>
#endif


/*
 *  Player Agent Packets
 */

inline void PlayerAgent_Snapshot::handle()
{
    #if DC_CLIENT
    ClientState::playerAgent_state.handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
    #endif
    //printf("Received Agent_state_message packet: agent_id= %i \n", id);
    //printf("seq= %i \n", seq);
    return;
}

// Server -> Client handlers
#if DC_CLIENT

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
    //int voxel[3];
    //voxel[0] = vx;
    //voxel[1] = vy;
    //voxel[2] = vz;
    //int voxel_blast_radius = 1;
    //if (target_type == OBJECT_AGENT)
    //{
        //Agent_state* target = ClientState::agent_list->get(target_id);
        //if (target->status.team == a->status.team) return;
        //voxel_blast_radius = 3;
    //}
    //else if (target_type == OBJECT_MONSTER_BOMB)
    //{
        //voxel_blast_radius = 2;
    //}
    //destroy_object_voxel(target_id, target_type, target_part, voxel, voxel_blast_radius);
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
    //int voxel[3];
    //voxel[0] = vx;
    //voxel[1] = vy;
    //voxel[2] = vz;
    //int voxel_blast_radius = 1;
    //if (target_type == OBJECT_AGENT)
    //{
        //Agent_state* target = ClientState::agent_list->get(target_id);
        //if (target->status.team == a->status.team) return;
        //voxel_blast_radius = 3;
    //}
    //else if (target_type == OBJECT_MONSTER_BOMB)
    //{
        //voxel_blast_radius = 2;
    //}
    //destroy_object_voxel(target_id, target_type, target_part, voxel, voxel_blast_radius);
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

inline void identified_StoC::handle()
{
    Agent_state* a = ClientState::playerAgent_state.you;
    if (a == NULL)
    {
        printf("identified_StoC -- identified as %s but player agent not assigned\n", name);
        return;
    }
    ClientState::playerAgent_state.was_identified();
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

        #if DC_CLIENT
        VersionMismatchBox(DC_VERSION, version);
        //input_state.quit = true;
        //input_state.confirm_quit = true;
        #endif
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

//inline void destroy_voxel_StoC::handle()
//{
    //int voxel[3] = { x,y,z };
    //destroy_object_voxel(entity_id, entity_type, entity_part, voxel, radius);
//}


inline void Agent_cs_CtoS::handle() {}
inline void hit_block_CtoS::handle() {}
inline void hitscan_object_CtoS::handle() {}
inline void hitscan_block_CtoS::handle() {}
inline void hitscan_none_CtoS::handle() {}
inline void ThrowGrenade_CtoS::handle(){}
inline void agent_set_block_CtoS::handle() {}
//#if !PRODUCTION
inline void admin_set_block_CtoS::handle() {}
//#endif
inline void place_spawner_CtoS::handle(){}
inline void place_turret_CtoS::handle(){}
inline void melee_object_CtoS::handle(){}
inline void melee_none_CtoS::handle(){}
inline void identify_CtoS::handle(){}
inline void ping_CtoS::handle(){}
inline void ping_reliable_CtoS::handle(){}
inline void choose_spawn_location_CtoS::handle(){}
inline void request_agent_name_CtoS::handle(){}
inline void request_remaining_state_CtoS::handle() {}
inline void agent_camera_state_CtoS::handle() {}
#endif

// Client -> Server handlers
#if DC_SERVER

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
inline void agent_name_StoC::handle() {}
inline void identified_StoC::handle(){}
inline void ping_StoC::handle(){}
inline void ping_reliable_StoC::handle(){}
inline void agent_conflict_notification_StoC::handle(){}
inline void version_StoC::handle(){}
inline void client_disconnected_StoC::handle(){}
inline void spawn_location_StoC::handle(){}
//inline void destroy_voxel_StoC::handle(){}

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
        
    class Agent_cs_StoC M;
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
    if (z < 0 || z >= map_dim.z) return;
    if (z == 0) return;
    
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }

    x = translate_point(x);
    y = translate_point(y);
    
    if (a->status.team == 0) return;
    //if (!a->weapons.pick.fire()) return;
    agent_hit_block_StoC msg;
    msg.id = a->id;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.broadcast();
    
    // TODO -- load this from dat
    //t_map::TerrainModificationAction tma = Item::get_item_terrain_modification_enum(weapon_type);

    if (z <= 0) return; // dont damage floor
    int block = t_map::get(x,y,z);
    if (block == 0) return;
    int block_damage = Item::get_item_block_damage(weapon_type, block);
    if (block_damage <= 0) return;
    t_map::apply_damage_broadcast(x,y,z, block_damage, t_map::TMA_PICK);
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
    //if (!a->weapons.laser.fire()) return;

    Agent_state* agent = NULL;
    const int obj_dmg = randrange(10,25);
    //int voxel[3] = { vx,vy,vz };

    using Objects::Object;
    Object* obj = NULL;

    using Components::OwnerComponent;
    using Components::TeamComponent;
    using Components::HealthComponent;
    using Components::MotionTargetingComponent;
    OwnerComponent* owner;
    TeamComponent* team;
    HealthComponent* health;
    MotionTargetingComponent* motion_targeting;
    int owner_id = NO_AGENT;
    int team_id = NO_TEAM;

    AgentState s = a->get_state();
    switch (type)
    {
        case OBJECT_AGENT:
            agent = ServerState::agent_list->get(id);
            if (agent == NULL || agent->vox == NULL) return;
            agent->vox->update(s.x, s.y, s.z, s.theta, s.phi);
            // apply damage
            agent->status.apply_hitscan_laser_damage_to_part(part, a->id, a->type);
            //destroy_object_voxel(agent->id, agent->type, part, voxel, 3);     
            break;

 
        case OBJECT_MONSTER_BOMB:
        case OBJECT_AGENT_SPAWNER:
        case OBJECT_TURRET:
        case OBJECT_MONSTER_BOX:
        case OBJECT_MONSTER_SPAWNER:
            obj = Objects::get((ObjectType)type, id);
            if (obj == NULL) return;

            owner = (OwnerComponent*)obj->get_component_interface(COMPONENT_INTERFACE_OWNER);
            if (owner != NULL) owner_id = owner->get_owner();

            team = (TeamComponent*)obj->get_component_interface(COMPONENT_INTERFACE_TEAM);
            if (team != NULL) team_id = team->get_team();

            if ((team_id == a->status.team && owner_id != NO_AGENT)
              && owner_id != a->id)   // TODO -- kill rule per object?
                return; // teammates cant kill turrets/spawners, unless object is unowned
                
            // apply damage
            health = (HealthComponent*)obj->get_component_interface(COMPONENT_INTERFACE_HEALTH);
            if (health != NULL) health->take_damage(obj_dmg);

            motion_targeting = (MotionTargetingComponent*)obj->get_component(COMPONENT_MOTION_TARGETING);
            if (motion_targeting != NULL && motion_targeting->target_type == OBJECT_NONE)
                motion_targeting->set_target(OBJECT_AGENT, a->id);
            
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

    if (a->status.team == 0) return;

    // get collision point on block surface (MOVE THIS TO A BETTER SPOT)
    // send to clients
    int collision[3];
    int pre_collision[3];
    int side[3];
    int cube;
    const float max_l = 500.0f;
    float distance=0.0f;

    float f[3];
    a->forward_vector(f);

    Vec3 p = a->get_camera_position();

    int collided = _ray_cast6(p.x, p.y, p.z, f[0], f[1], f[2], max_l, &distance, collision, pre_collision, &cube, side);
    if (!collided) return;
    // pt of collision
    p.x += f[0] * distance;
    p.y += f[1] * distance;
    p.z += f[2] * distance;

    p = translate_position(p);

    int cube_side = get_cube_side_from_side_array(side);

    agent_shot_block_StoC msg;
    msg.id = a->id;
    msg.x = p.x;
    msg.y = p.y;
    msg.z = p.z;
    msg.cube = cube;
    msg.side = cube_side;
    msg.broadcast();

    // damage block
    // WARNING:
    // *must* call this after raycasting, or you will be raycasting altered terrain
    if (p.z <= 0) return; // dont damage floor
    int block = t_map::get(p.x, p.y, p.z);
    if (block == 0) return;
    int weapon_block_damage = Item::get_item_block_damage(Item::get_item_type((char*)"laser_rifle"), block);
    if (weapon_block_damage <= 0) return;
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
    //if (!a->weapons.laser.fire()) return;
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

    Agent_state* agent = NULL;
    const int obj_dmg = Item::get_item_object_damage(weapon_type);
    //int voxel[3] = { vx,vy,vz };

    using Objects::Object;
    Object* obj = NULL;

    using Components::OwnerComponent;
    using Components::TeamComponent;
    using Components::HealthComponent;
    OwnerComponent* owner;
    TeamComponent* team;
    HealthComponent* health;
    int owner_id = NO_AGENT;
    int team_id = NO_TEAM;
    bool died = true;   // assume hitting object kills it, unless object says otherwise
    
    switch (type)
    {
        case OBJECT_AGENT:
            agent = ServerState::agent_list->get(id);
            if (agent==NULL) return;
            // apply damage
            agent->status.apply_hitscan_laser_damage_to_part(part, a->id, a->type);
            //destroy_object_voxel(agent->id, agent->type, part, voxel, 3);
            break;

        case OBJECT_MONSTER_BOMB:
        case OBJECT_AGENT_SPAWNER:
        case OBJECT_TURRET:
        case OBJECT_MONSTER_BOX:
        case OBJECT_MONSTER_SPAWNER:
            obj = Objects::get((ObjectType)type, id);
            if (obj == NULL) return;

            owner = (OwnerComponent*)obj->get_component_interface(COMPONENT_INTERFACE_OWNER);
            if (owner != NULL) owner_id  = owner->get_owner();

            team = (TeamComponent*)obj->get_component_interface(COMPONENT_INTERFACE_TEAM);
            if (team != NULL) team_id =  team->get_team();

            if ((team_id == a->status.team && owner_id != NO_AGENT)
              && owner_id != a->id)   // TODO -- kill rule per object?
                return; // teammates cant kill turrets/spawners, unless object is unowned
                
            // apply damage
            health = (HealthComponent*)obj->get_component_interface(COMPONENT_INTERFACE_HEALTH);
            if (health != NULL)
            {
                health->take_damage(obj_dmg);
                died = health->did_die();
            }

            if (died)
            {
                if (obj->type == OBJECT_MONSTER_BOMB)
                    a->status.kill_slime(); // TODO, de-type this
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
    //if (!a->weapons.pick.fire()) return;
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
    //if (!a->weapons.grenades.fire()) return;
    agent_threw_grenade_StoC msg;
    msg.id = a->id;
    msg.broadcast();

    Vec3 n = vec3_init(vx,vy,vz);
    normalize_vector(&n);
    static const float PLAYER_ARM_FORCE = 15.0f; // load from dat later
    //create grenade
    n = vec3_scalar_mult(n, PLAYER_ARM_FORCE);
    Particle::Grenade* g = Particle::grenade_list->create();
    if (g==NULL) return;
    g->set_state(x,y,z, n.x, n.y, n.z);
    g->owner = a->id;
    g->broadcast();
}

inline void agent_set_block_CtoS::handle()
{
    if (z < 0 || z >= map_dim.z) return;
    if (z == 0) return;     // dont set bottom layer

    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    // fire block applier
    if (a->status.team == 0) return;

    Item::Item* placer = Item::get_item((ItemID)placer_id);
    if (placer == NULL) return;
    Item::ItemAttribute* attr = Item::get_item_attributes(placer->type);
    int val = attr->placer_block_type_id;
    
    // do block place checks here later
    // problem is, fire/(decrement ammo) packet is separate, and isnt aware of this failure

    x = translate_point(x);
    y = translate_point(y);

    // dont set on existing block
    if (t_map::get(x,y,z) != 0) return;

    // check this player first, most likely to be colliding
    bool collides = false;
    _set(x,y,z, val); // set temporarily to test against
    if (agent_collides_terrain(a))
        collides = true;
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

    if (collides) return;
    
    Toolbelt::use_block_placer(a->id, (ItemID)placer_id);
    t_map::broadcast_set_block(x,y,z, val);
    agent_placed_block_StoC msg;
    msg.id = a->id;
    msg.broadcast();

    t_map::broadcast_set_block_action(x,y,z, val, t_map::TMA_PLACE_BLOCK);
}

//#if !PRODUCTION
inline void admin_set_block_CtoS::handle()
{
    if (z < 0 || z >= map_dim.z) return;

    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }

    x = translate_point(x);
    y = translate_point(y);

    // do block place checks here later
    // problem is, fire/(decrement ammo) packet is separate, and isnt aware of this failure

    // check this player first, most likely to be colliding
    _set(x,y,z, val); // set temporarily to test against
    bool collides = agent_collides_terrain(a);
    if (!collides)
    {   // check the rest of the players
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

    if (collides) return;
    
    t_map::broadcast_set_block(x,y,z, val);
    agent_placed_block_StoC msg;
    msg.id = a->id;
    msg.broadcast();

    t_map::broadcast_set_block_action(x,y,z, val, t_map::TMA_PLACE_BLOCK);
}
//#endif

#define ITEM_PLACEMENT_Z_DIFF_LIMIT 3

static Objects::Object* place_object_handler(ObjectType type, int x, int y, int z, int owner_id, int team_id)
{
    if (Objects::point_occupied_by_type(OBJECT_TURRET, x, y, z)) return NULL;
    if (Objects::point_occupied_by_type(OBJECT_AGENT_SPAWNER, x, y, z)) return NULL;

    // zip down
    int new_z = t_map::get_highest_open_block(x,y);
    if (z - new_z > ITEM_PLACEMENT_Z_DIFF_LIMIT || z - new_z < 0) return NULL;
    if (Objects::point_occupied_by_type(OBJECT_TURRET, x, y, new_z)) return NULL;
    if (Objects::point_occupied_by_type(OBJECT_AGENT_SPAWNER, x, y, new_z)) return NULL;

    using Objects::Object;
    Object* object = Objects::create(type);
    if (object == NULL) return NULL;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL) physics->set_position(vec3_init(x+0.5f,y+0.5f,new_z));

    using Components::TeamComponent;
    TeamComponent* team = (TeamComponent*)object->get_component_interface(COMPONENT_INTERFACE_TEAM);
    if (team != NULL) team->set_team(team_id);

    using Components::OwnerComponent;
    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    if (owner != NULL) owner->set_owner(owner_id);

    return object;
}

inline void place_spawner_CtoS::handle()
{
    ObjectType type = OBJECT_AGENT_SPAWNER;
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("place_turret_CtoS:: Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    if (!Components::agent_spawner_component_list->team_spawner_available(a->status.team)) return;

    Objects::Object* obj = place_object_handler(type, x,y,z, a->id, a->status.team);
    if (obj == NULL) return;
    Components::agent_spawner_component_list->assign_team_index(obj);
    Objects::ready(obj);
}

inline void place_turret_CtoS::handle()
{
    ObjectType type = OBJECT_TURRET;
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("place_turret_CtoS:: Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    if (a->status.team == 0) return;
    
    Objects::Object* obj = place_object_handler(type, x,y,z, a->id, a->status.team);
    if (obj == NULL) return;
    Objects::ready(obj);
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
    printf("Received name %s\n", name);

    unsigned int len = sanitize_player_name(name);
    
    if (len == 0)
        strcpy(name, DEFAULT_PLAYER_NAME);

    if (len >= PLAYER_NAME_MAX_LENGTH)
        name[PLAYER_NAME_MAX_LENGTH-1] = '\0';

    int breakout = 0;   // safeguard against infinite loop
    const int breakout_limit = 100;
    while (!ServerState::agent_list->name_available(name))
    {
        breakout++;
        if (breakout % breakout_limit == 0)
        {
            printf("ERROR: identify_CtoS::handle() -- failed to find a valid agent name after %d attempts\n", breakout);
            return; // bailout, something is wrong
        }
        adjust_name(name, len);
    }

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

inline void request_remaining_state_CtoS::handle()
{   // client reporting received agent; send metadata
    NetPeerManager* client = NetServer::clients[client_id];
    if (client == NULL)
    {
        printf("request_remaining_state_CtoS::handle() -- NetPeerManager not found for %d\n", client_id);
        return;
    }
    client->send_remaining_state();
}

inline void agent_camera_state_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->id != id) return;
    a->set_camera_state(x,y,z, theta,phi);
    a->camera_ready = true;
    //printf("Camera state received: %0.2f,%0.2f,%0.2f\n", x,y,z);
}


#endif
