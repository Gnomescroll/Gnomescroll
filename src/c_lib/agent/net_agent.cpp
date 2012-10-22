#include "net_agent.hpp"

#include <agent/_state.hpp>

#include <agent/agent.hpp>
#include <common/defines.h>

#if DC_CLIENT
#include <state/client_state.hpp>
#include <common/time/physics_timer.hpp>
#include <chat/client.hpp>
#include <chat/interface.hpp>
#include <entity/objects/fabs/constants.hpp>
#include <SDL/SDL_functions.hpp>
#endif

#if DC_SERVER
#include <state/server_state.hpp>
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
}

// Server -> Client handlers
#if DC_CLIENT

inline void SendClientId_StoC::handle()
{
    NetClient::Server.client_id = (ClientID)this->client_id;
    if (ClientState::playerAgent_state.you != NULL)
        ClientState::playerAgent_state.you->client_id = (ClientID)client_id;
}

inline void Agent_state_message::handle()
{
    Agent* a = Agents::get_agent((AgentID)id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    #if DC_SERVER
    z = clamp_z(z);
    #endif
    a->handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
}

inline void Agent_teleport_message::handle()
{
    Agent* a = Agents::get_agent((AgentID)id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    // reset camera angle
    if (a->is_you())
    {
        a->initial_teleport = true;
        if (agent_camera != NULL)
            agent_camera->set_angles(theta, phi);
    }
    a->set_state(x,y,z, vx,vy,vz);
    a->set_angles(theta, phi);
    // force update vox
    if (a->vox != NULL)
        a->vox->update(x,y,z,theta,phi);
}

//Agent control state, server to client
inline void Agent_cs_StoC::handle()
{
    Agent* a = Agents::get_agent((AgentID)id);
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
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->event.took_damage(dmg);
}

inline void agent_shot_object_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->event.fired_weapon_at_object(target_id, (ObjectType)target_type, target_part);
}

inline void agent_shot_block_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->event.fired_weapon_at_block(x,y,z, cube, side);
}

inline void agent_shot_nothing_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->event.fired_weapon_at_nothing();
}

inline void agent_melee_object_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->event.melee_attack_object(target_id, (ObjectType)target_type, target_part);
}

inline void agent_melee_nothing_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->event.melee_attack_nothing();
}

inline void agent_hit_block_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->event.hit_block();
}

inline void agent_threw_grenade_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->event.threw_grenade();
}

inline void agent_placed_block_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->event.placed_block();
}

inline void agent_dead_StoC::handle()
{
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->event.life_changing((bool)this->dead);
}

inline void agent_health_StoC::handle()
{
    Agent* a = Agents::get_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    if (health > a->status.health)
        a->event.healed(health - a->status.health);
    a->status.health = health;
}

inline void agent_create_StoC::handle()
{
    Agent* a = Agents::create_agent((AgentID)this->id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->client_id = (ClientID)this->client_id;
    a->status.set_name(this->username);
    a->status.set_color(this->color);
    a->event.name_set();

    GS_ASSERT(chat_client != NULL);
    if (chat_client != NULL)
    {
        const char fmt[] = "%s has joined the game";
        char* msg = (char*)calloc(strlen(fmt) + strlen(this->username) - 2 + 1, sizeof(char));
        sprintf(msg, fmt, this->username);
        chat_client->send_system_message(msg);
        free(msg);
    }
}

inline void agent_destroy_StoC::handle()
{
    Agents::destroy_agent((AgentID)id);
}

inline void PlayerAgent_id_StoC::handle()
{
    ClientState::set_PlayerAgent_id((AgentID)this->id);
}

inline void AgentKills_StoC::handle()
{
    Agent* a = Agents::get_agent((AgentID)this->id);
    if(a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->status.kills = kills;
}

inline void AgentDeaths_StoC::handle()
{
    Agent* a = Agents::get_agent((AgentID)this->id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->status.deaths = deaths;
}

inline void AgentSuicides_StoC::handle()
{
    Agent* a = Agents::get_agent((AgentID)this->id);
    if (a == NULL)
    {
        printf("Agent %d not found. message_id=%d\n", id, message_id);
        return;
    }
    a->status.suicides = suicides;
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

    Agent* a = Agents::get_agent((AgentID)this->attacker);
    Agent* b = Agents::get_agent((AgentID)this->victim);
    
    char unknown_name[] = "Someone";
    char *a_name = (a == NULL) ? unknown_name : a->status.name;
    char *b_name = (b == NULL) ? unknown_name : b->status.name;

    char* msg = (char*)calloc(512, sizeof(char));
    switch (method)
    {
        case DEATH_KILLME:
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
    printf("Client Version: %d\n", GS_VERSION);
    printf("Server Version: %d\n", version);
    NetClient::Server.version = version;
    if (GS_VERSION != version)
    {
        Hud::set_error_status(GS_ERROR_VERSION_MISMATCH);
        printf("WARNING: Version mismatch\n");
        #if DC_CLIENT
        VersionMismatchBox(GS_VERSION, version);
        #endif
    }
    else
        chat_client->send_system_message("Connected to server");
}

inline void client_disconnected_StoC::handle()
{
    const char fmt[] = "%s has left the game";
    size_t msg_len = strlen(fmt) + strlen(name) - 2 + 1;
    char* msg = (char*)malloc(msg_len * sizeof(char));
    int wrote = snprintf(msg, msg_len, fmt, name);
    GS_ASSERT(wrote < (int)msg_len);
    chat_client->send_system_message(msg);
    free(msg);
}

inline void set_spawner_StoC::handle()
{
    using ClientState::playerAgent_state;
    if (playerAgent_state.you == NULL) return;

    ASSERT_VALID_SPAWNER_ID(this->spawner_id);
    IF_INVALID_SPAWNER_ID(this->spawner_id) return;
    
    // de-color old spawner
    if (playerAgent_state.you->status.spawner != BASE_SPAWN_ID)
    {
        Objects::Object* obj = Objects::get(OBJECT_AGENT_SPAWNER, playerAgent_state.you->status.spawner);
        if (obj != NULL)
        {
            using Components::VoxelModelComponent;
            VoxelModelComponent* vox = (VoxelModelComponent*)obj->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
            GS_ASSERT(vox != NULL);
            if (vox != NULL && vox->vox != NULL)
                vox->vox->fill_color(Objects::DEACTIVATED_SPAWNER_COLOR);
        }
    }

    // color new spawner
    if (this->spawner_id != BASE_SPAWN_ID)    // TODO -- remove this check, once base is removed (if it is)
    {
        Objects::Object* obj = Objects::get(OBJECT_AGENT_SPAWNER, this->spawner_id);
        GS_ASSERT(obj != NULL);
        if (obj != NULL)
        {
            using Components::VoxelModelComponent;
            VoxelModelComponent* vox = (VoxelModelComponent*)obj->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
            GS_ASSERT(vox != NULL);
            if (vox != NULL && vox->vox != NULL)
                vox->vox->fill_color(Objects::ACTIVATED_SPAWNER_COLOR);
        }
    }

    playerAgent_state.you->event.set_spawner(this->spawner_id);
}

inline void agent_color_StoC::handle()
{
    Agent* a = Agents::get_agent((AgentID)this->agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    
    a->status.set_color(this->color);
    
    if (this->agent_id == ClientState::playerAgent_state.agent_id)
    {
        const char fmt[] = "Your color is now %d %d %d\n";
        size_t len = sizeof(fmt) + 3*3 - 2*3 + 1;
        char* msg = (char*)malloc(len * sizeof(char));
        snprintf(msg, len, fmt, color.r, color.g, color.b);
        chat_client->send_system_message(msg);
        free(msg);
    }
}

inline void Agent_cs_CtoS::handle() {}
inline void hit_block_CtoS::handle() {}
inline void hitscan_object_CtoS::handle() {}
inline void hitscan_block_CtoS::handle() {}
inline void hitscan_none_CtoS::handle() {}
inline void ThrowGrenade_CtoS::handle(){}
inline void agent_set_block_CtoS::handle() {}
inline void admin_set_block_CtoS::handle() {}
inline void place_spawner_CtoS::handle(){}
inline void place_turret_CtoS::handle(){}
inline void melee_object_CtoS::handle(){}
inline void melee_none_CtoS::handle(){}
inline void ping_CtoS::handle(){}
inline void ping_reliable_CtoS::handle(){}
inline void choose_spawner_CtoS::handle(){}
inline void agent_camera_state_CtoS::handle() {}
inline void version_CtoS::handle(){}
inline void killme_CtoS::handle() {}
inline void colorme_CtoS::handle() {}
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
inline void ping_StoC::handle(){}
inline void ping_reliable_StoC::handle(){}
inline void agent_conflict_notification_StoC::handle(){}
inline void version_StoC::handle(){}
inline void client_disconnected_StoC::handle(){}
inline void set_spawner_StoC::handle(){}
inline void agent_color_StoC::handle() {}

//for benchmarking
//static int _total = 0;
//static const int a_DEBUG = 1;

inline void version_CtoS::handle()
{
    NetServer::users->record_client_version(client_id, version);
}

inline void colorme_CtoS::handle()
{
    Agent* a = NetServer::agents[this->client_id];
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->status.set_color(this->color);
}

inline void killme_CtoS::handle()
{
    Agent* a = NetServer::agents[this->client_id];
    if (a == NULL) return;

    a->status.die(a->id, a->type, DEATH_KILLME);
}

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

    Agent* A = NetServer::agents[client_id];
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
    // comparisons not needed due to value range of data type
    //if (z < 0 || z >= map_dim.z) return;
    //if (z == 0) return;
    
    Agent* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }

    x = translate_point(x);
    y = translate_point(y);
    z = clamp_z(z);

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
    Agent* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }

    if (type == OBJECT_AGENT)
    {
        Agent* agent = Agents::get_agent((AgentID)this->id);
        if (agent == NULL || agent->vox == NULL) return;
        force_update_agent_vox(a);
        // apply damage
        agent->status.apply_hitscan_laser_damage_to_part(part, a->id, a->type);
    }
    else
    {
        class Objects::Object* obj = Objects::get((ObjectType)type, id);
        if (obj == NULL) return;

        // apply damage
        const int obj_dmg = randrange(10,25);   // TODO -- weapon based
        
        using Components::HealthComponent;
        HealthComponent* health = (HealthComponent*)obj->get_component_interface(COMPONENT_INTERFACE_HEALTH);
        if (health != NULL) health->take_damage(obj_dmg);

        // set target on person attacking
        using Components::MotionTargetingComponent;
        MotionTargetingComponent* motion_targeting = (MotionTargetingComponent*)
            obj->get_component(COMPONENT_MOTION_TARGETING);
        if (motion_targeting != NULL)
        {
            if (motion_targeting->target_type == OBJECT_NONE)
                motion_targeting->set_target(OBJECT_AGENT, a->id);
        }
        else
        {
            using Components::AgentTargetingComponent;
            AgentTargetingComponent* agent_targeting = (AgentTargetingComponent*)
                obj->get_component(COMPONENT_AGENT_TARGETING);
            if (agent_targeting != NULL)
            {
                if (agent_targeting->target_type == OBJECT_NONE)
                    agent_targeting->set_target(a->id);
                else    // reset ticks locked
                    agent_targeting->ticks_locked = 0;

                using Components::StateMachineComponent;
                StateMachineComponent* state_machine = (StateMachineComponent*)
                    obj->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
                if (state_machine != NULL && state_machine->router != NULL)
                    state_machine->router(obj, STATE_CHASE_AGENT);
            }
        }
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
    Agent* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }

    z = clamp_z(z);

    // get collision point on block surface (MOVE THIS TO A BETTER SPOT)
    // send to clients
    int collision[3];
    int pre_collision[3];
    int side[3];
    int cube;
    const float max_l = 500.0f;
    float distance=0.0f;

    Vec3 f = a->forward_vector();
    Vec3 p = a->get_camera_position();

    int collided = _ray_cast6(p.x, p.y, p.z, f.x, f.y, f.z, max_l, &distance, collision, pre_collision, &cube, side);
    if (!collided) return;
    // pt of collision
    f = vec3_scalar_mult(f, distance);
    p = vec3_add(p,f);
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
    static int laser_rifle_type = Item::get_item_type("laser_rifle");
    int weapon_block_damage = Item::get_item_block_damage(laser_rifle_type, block);
    if (weapon_block_damage <= 0) return;
    t_map::apply_damage_broadcast(x,y,z, weapon_block_damage, t_map::TMA_LASER);
}

inline void hitscan_none_CtoS::handle()
{
    Agent* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    agent_shot_nothing_StoC msg;
    msg.id = a->id;
    msg.broadcast();
}

inline void melee_object_CtoS::handle()
{
    Agent* a = NetServer::agents[client_id];
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    
    if (type == OBJECT_AGENT)
    {
        class Agent* agent = Agents::get_agent((AgentID)this->id);
        if (agent==NULL) return;
        agent->status.apply_mining_laser_damage_to_part(part, a->id, a->type);
    }
    else
    {
        class Objects::Object* obj = Objects::get((ObjectType)type, id);
        if (obj == NULL) return;

        // apply damage
        const int obj_dmg = Item::get_item_object_damage(weapon_type);
        using Components::HealthComponent;
        HealthComponent* health = (HealthComponent*)
            obj->get_component_interface(COMPONENT_INTERFACE_HEALTH);
        if (health != NULL)
            health->take_damage(obj_dmg);
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
    Agent* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    agent_melee_nothing_StoC msg;
    msg.id = a->id;
    msg.broadcast();
}

inline void ThrowGrenade_CtoS::handle()
{
    Agent* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }
    agent_threw_grenade_StoC msg;
    msg.id = a->id;
    msg.broadcast();

    z = clamp_z(z);

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
    // comparisons not needed due to value range of data type
    //if (z < 0 || z >= map_dim.z) return;
    //if (z == 0) return;     // dont set bottom layer

    Agent* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }

    // fire block applier
    Item::Item* placer = Item::get_item((ItemID)placer_id);
    if (placer == NULL) return;
    Item::ItemAttribute* attr = Item::get_item_attributes(placer->type);
    int val = attr->block_type_id;

    IF_INVALID_CUBE_ID(val) return;
    if (t_map::isErrorBlock(val)) return;
    if (!t_map::isInUse(val)) return;
    
    // do block place checks here later
    // problem is, fire/(decrement ammo) packet is separate, and isnt aware of this failure

    x = translate_point(x);
    y = translate_point(y);
    z = clamp_z(z);

    // dont set on existing block
    if (!t_map::block_can_be_placed(x,y,z,val)) return;

    // check this player first, most likely to be colliding
    bool collides = false;
    t_map::set_fast(x,y,z, val); // set temporarily to test against
    if (agent_collides_terrain(a))
        collides = true;
    else
    {
        for (unsigned int i=0; i<Agents::agent_list->max; i++)
        {
            Agent* agent = &Agents::agent_list->objects[i];
            if (agent->id == Agents::agent_list->null_id || agent == a) continue;
            if (agent_collides_terrain(agent))
            {
                collides = true;
                break;
            }
        }
    }
    t_map::set_fast(x,y,z,0);  // unset

    if (collides) return;
    
    Toolbelt::use_block_placer(a->id, (ItemID)placer_id);
    t_map::broadcast_set_block(x,y,z, val);
    agent_placed_block_StoC msg;
    msg.id = a->id;
    msg.broadcast();

    t_map::broadcast_set_block_action(x,y,z, val, t_map::TMA_PLACE_BLOCK);

    /*
        Handle Special Block Placement
    */
    static int _control_node = t_map::dat_get_cube_id("control_node");

    if(val == _control_node) t_map::add_control_node(x,y,z);
}

//#if !PRODUCTION
inline void admin_set_block_CtoS::handle()
{
    // comparisons not needed due to value range of data type
    //if (z < 0 || z >= map_dim.z) return;

    Agent* a = NetServer::agents[client_id];
    GS_ASSERT(a != NULL);
    if (a == NULL) return;

    IF_INVALID_CUBE_ID(val) return;
    if (t_map::isErrorBlock(val)) return;
    if (!t_map::isInUse(val)) return;

    x = translate_point(x);
    y = translate_point(y);
    z = clamp_z(z);

    // TODO -- when this is a /real/ admin tool, remove this check
    // since we're giving it to players, do this check
    if (!t_map::block_can_be_placed(x,y,z,val)) return;

    // check this player first, most likely to be colliding
    t_map::set_fast(x,y,z, val); // set temporarily to test against
    bool collides = agent_collides_terrain(a);
    if (!collides)
    {   // check the rest of the players
        for (unsigned int i=0; i<Agents::agent_list->max; i++)
        {
            Agent* agent = &Agents::agent_list->objects[i];
            if (agent->id == Agents::agent_list->null_id || agent == a) continue;
            if (agent_collides_terrain(agent))
            {
                collides = true;
                break;
            }
        }
    }
    t_map::set_fast(x,y,z,0);  // unset

    if (collides) return;
    
    t_map::broadcast_set_block(x,y,z, val);
    agent_placed_block_StoC msg;
    msg.id = a->id;
    msg.broadcast();

    t_map::broadcast_set_block_action(x,y,z, val, t_map::TMA_PLACE_BLOCK);
}
//#endif

static Objects::Object* place_object_handler(ObjectType type, int x, int y, int z, AgentID owner_id)
{
    if (Objects::point_occupied_by_type(OBJECT_TURRET, x, y, z)) return NULL;
    if (Objects::point_occupied_by_type(OBJECT_AGENT_SPAWNER, x, y, z)) return NULL;

    // zip down
    static const int ITEM_PLACEMENT_Z_DIFF_LIMIT = 3;

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

    using Components::OwnerComponent;
    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    if (owner != NULL) owner->set_owner(owner_id);
    
    return object;
}

inline void place_spawner_CtoS::handle()
{
    ObjectType type = OBJECT_AGENT_SPAWNER;
    Agent* a = NetServer::agents[client_id];
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    z = clamp_z(z);
    Objects::Object* obj = place_object_handler(type, x,y,z, a->id);
    if (obj == NULL) return;
    Objects::ready(obj);
}

inline void place_turret_CtoS::handle()
{
    ObjectType type = OBJECT_TURRET;
    Agent* a = NetServer::agents[client_id];
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    z = clamp_z(z);
    Objects::Object* obj = place_object_handler(type, x,y,z, a->id);
    if (obj == NULL) return;
    Objects::ready(obj);
}

inline void choose_spawner_CtoS::handle()
{
    Agent* a = NetServer::agents[this->client_id];
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    a->status.set_spawner(spawner_id);
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

inline void agent_camera_state_CtoS::handle()
{
    Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->id != id) return;
    z = clamp_z(z);
    a->set_camera_state(x,y,z, theta,phi);
    a->camera_ready = true;
}

#endif
