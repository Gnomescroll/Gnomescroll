#include "net_agent.hpp"

#include <agent/_state.hpp>
#include <agent/agent.hpp>
#include <common/defines.h>
#include <state/state.hpp>

#if DC_CLIENT
# include <common/time/physics_timer.hpp>
# include <chat/_interface.hpp>
# include <SDL/SDL_functions.hpp>
#endif

#if DC_SERVER
# include <t_map/t_map.hpp>
# include <physics/ray_trace/ray_trace.hpp>
#endif


/*
 *  Player Agents::Agent Packets
 */

inline void player_agent_snapshot_StoC::handle()
{
    #if DC_CLIENT
    ClientState::player_agent.handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
    #endif
}

// Server -> Client handlers
#if DC_CLIENT

inline void send_client_id_StoC::handle()
{
    NetClient::Server.client_id = (ClientID)this->client_id;
}

inline void agent_state_StoC::handle()
{
    Agents::Agent* a = Agents::get_agent((AgentID)id);
    if (a == NULL) return;
    a->handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
}

inline void agent_teleport_StoC::handle()
{
    Agents::Agent* a = Agents::get_agent((AgentID)id);
    IF_ASSERT(a == NULL) return;
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
        a->vox->update(vec3_init(x, y, z), theta, phi);
}

inline void agent_control_state_StoC::handle()
{
    Agents::Agent* a = Agents::get_agent((AgentID)id);
    if (a == NULL) return;
    a->handle_control_state(seq, cs, theta, phi);
    ClientState::player_agent.handle_net_control_state(seq, cs, theta, phi);
}

inline void agent_hitscan_entity_StoC::handle()
{
    if (id == ClientState::player_agent.agent_id) return;   // ignore you, should have played locally before transmission
    Agents::Agent* a = Agents::get_agent((AgentID)this->id);
    IF_ASSERT(a == NULL) return;
    a->event.fired_weapon_at_object(target_id, (EntityType)target_type);
}

inline void agent_hitscan_block_StoC::handle()
{
    if (id == ClientState::player_agent.agent_id) return;   // ignore you, should have played locally before transmission
    Agents::Agent* a = Agents::get_agent((AgentID)this->id);
    IF_ASSERT(a == NULL) return;
    CubeType cube = t_map::get(this->position);
    const Vec3 pos = vec3_scalar_add(vec3_init(this->position), 0.5f);
    int side = a->get_facing_side(pos);
    a->event.fired_weapon_at_block(pos, cube, side);
}

inline void agent_hitscan_nothing_StoC::handle()
{
    if (id == ClientState::player_agent.agent_id) return;   // ignore you, should have played locally before transmission
    Agents::Agent* a = Agents::get_agent((AgentID)this->id);
    IF_ASSERT(a == NULL) return;
    a->event.fired_weapon_at_nothing();
}

inline void agent_launched_projectile_StoC::handle()
{
    if (id == ClientState::player_agent.agent_id) return;   // ignore you, should have played locally before transmission
    Agents::Agent* a = Agents::get_agent((AgentID)this->id);
    IF_ASSERT(a == NULL) return;
    if (strcmp(Item::get_item_name(ItemType(this->item_type)),
               "plasma_grenade") == 0)
        a->event.threw_grenade();
}

inline void agent_placed_block_StoC::handle()
{
    if (id == ClientState::player_agent.agent_id) return;   // ignore you, should have played locally before transmission
    Agents::Agent* a = Agents::get_agent((AgentID)this->id);
    IF_ASSERT(a == NULL) return;
    a->event.placed_block();
}

inline void agent_dead_StoC::handle()
{
    Agents::Agent* a = Agents::get_agent((AgentID)this->id);
    IF_ASSERT(a == NULL) return;
    a->event.life_changing((bool)this->dead);
}

inline void agent_create_StoC::handle()
{
    Agents::Agent* a = Agents::create_agent((AgentID)this->id);
    IF_ASSERT(a == NULL) return;
    a->client_id = (ClientID)this->client_id;
    a->status.set_name(this->username);
    a->status.set_color(this->color);
    a->event.name_set();
    Chat::send_system_messagef("%s has joined the game", this->username);
}

inline void agent_destroy_StoC::handle()
{
    Agents::destroy_agent((AgentID)id);
}

inline void player_agent_id_StoC::handle()
{
    ClientState::set_player_agent_id((AgentID)this->id);
}

inline void agent_kills_StoC::handle()
{
    Agents::Agent* a = Agents::get_agent((AgentID)this->id);
    IF_ASSERT(a == NULL) return;
    a->status.kills = kills;
}

inline void agent_deaths_StoC::handle()
{
    Agents::Agent* a = Agents::get_agent((AgentID)this->id);
    IF_ASSERT(a == NULL) return;
    a->status.deaths = deaths;
}

inline void agent_suicides_StoC::handle()
{
    Agents::Agent* a = Agents::get_agent((AgentID)this->id);
    IF_ASSERT(a == NULL) return;
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

    Agents::Agent* a = Agents::get_agent((AgentID)this->attacker);
    Agents::Agent* b = Agents::get_agent((AgentID)this->victim);

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
                    sprintf(msg, "You got shot in the head by %s", a_name);
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

        case DEATH_PLASMAGEN:
            if (a != NULL && a->is_you())
                strcpy(msg, "You've been fried.");
            else
                sprintf(msg, "%s's skin melted off.", a_name);
            break;

        case DEATH_STARVATION:
            if (a != NULL && a->is_you())
                strcpy(msg, "You starved to death.");
            else
                sprintf(msg, "%s collapsed.", a_name);

        default: break;
    }

    Chat::send_system_message(msg);
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
        Chat::send_system_message("Connected to server");
}

inline void client_disconnected_StoC::handle()
{
    const char fmt[] = "%s has left the game";
    size_t msg_len = strlen(fmt) + strlen(name) - 2 + 1;
    char* msg = (char*)malloc(msg_len * sizeof(char));
    int wrote = snprintf(msg, msg_len, fmt, name);
    GS_ASSERT(wrote < (int)msg_len);
    Chat::send_system_message(msg);
    free(msg);
}

inline void set_spawner_StoC::handle()
{
    using ClientState::player_agent;
    class Agents::Agent* you = player_agent.you();
    if (you == NULL) return;

    EntityID spawner_id = EntityID(this->spawner_id);

    ASSERT_VALID_SPAWNER_ID(spawner_id);
    IF_INVALID_SPAWNER_ID(spawner_id) return;

    // de-color old spawner
    if (you->status.spawner != BASE_SPAWN_ID)
    {
        Entities::Entity* obj = Entities::get(ENTITY_AGENT_SPAWNER, you->status.spawner);
        if (obj != NULL)
        {
            auto vox = GET_COMPONENT_INTERFACE(VoxelModel, obj);
            GS_ASSERT(vox != NULL);
            if (vox != NULL && vox->vox != NULL)
                vox->vox->fill_color(DEACTIVATED_SPAWNER_COLOR);
        }
    }

    // color new spawner
    if (spawner_id != BASE_SPAWN_ID)    // TODO -- remove this check, once base is removed (if it is)
    {
        Entities::Entity* obj = Entities::get(ENTITY_AGENT_SPAWNER, spawner_id);
        GS_ASSERT(obj != NULL);
        if (obj != NULL)
        {
            auto vox = GET_COMPONENT_INTERFACE(VoxelModel, obj);
            GS_ASSERT(vox != NULL);
            if (vox != NULL && vox->vox != NULL)
                vox->vox->fill_color(ACTIVATED_SPAWNER_COLOR);
        }
    }

    if (you != NULL) you->event.set_spawner(spawner_id);
}

inline void agent_color_StoC::handle()
{
    Agents::Agent* a = Agents::get_agent((AgentID)this->agent_id);
    IF_ASSERT(a == NULL) return;

    a->status.set_color(this->color);

    if (this->agent_id == ClientState::player_agent.agent_id)
    {
        const char fmt[] = "Your color is now %d %d %d\n";
        size_t len = sizeof(fmt) + 3*3 - 2*3 + 1;
        char* msg = (char*)malloc(len * sizeof(char));
        snprintf(msg, len, fmt, color.r, color.g, color.b);
        Chat::send_system_message(msg);
        free(msg);
    }
}

inline void agent_control_state_CtoS::handle() {}
inline void hitscan_entity_CtoS::handle() {}
inline void hitscan_block_CtoS::handle() {}
inline void hitscan_mech_CtoS::handle() {}
inline void hitscan_none_CtoS::handle() {}
inline void launch_projectile_CtoS::handle(){}
inline void agent_set_block_CtoS::handle() {}
inline void place_spawner_CtoS::handle(){}
inline void place_turret_CtoS::handle(){}
inline void ping_CtoS::handle(){}
inline void ping_reliable_CtoS::handle(){}
inline void choose_spawner_CtoS::handle(){}
inline void agent_camera_state_CtoS::handle() {}
inline void version_CtoS::handle(){}
inline void killme_CtoS::handle() {}
inline void colorme_CtoS::handle() {}
inline void teleport_me_CtoS::handle() {}
inline void gohome_CtoS::handle() {}
#endif

// Client -> Server handlers
#if DC_SERVER

inline void send_client_id_StoC::handle() {}
inline void agent_kills_StoC::handle() {}
inline void agent_deaths_StoC::handle() {}
inline void agent_suicides_StoC::handle() {}
inline void agent_state_StoC::handle() {}
inline void agent_teleport_StoC::handle() {}
inline void agent_control_state_StoC::handle() {}
inline void agent_hitscan_entity_StoC::handle(){}
inline void agent_hitscan_block_StoC::handle(){}
inline void agent_hitscan_nothing_StoC::handle(){}
inline void agent_launched_projectile_StoC::handle(){}
inline void agent_placed_block_StoC::handle(){}
inline void agent_dead_StoC::handle() {}
inline void agent_create_StoC::handle() {}
inline void agent_destroy_StoC::handle() {}
inline void player_agent_id_StoC::handle() {}
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
    Agents::Agent* a = NetServer::agents[this->client_id];
    IF_ASSERT(a == NULL) return;
    a->status.set_color(this->color);
}

inline void killme_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[this->client_id];
    if (a == NULL) return;

    a->status.die(a->id, a->type, DEATH_KILLME);
}

inline void agent_control_state_CtoS::handle()
{
    //printf("cs_CtoS: seq= %i \n", seq);

    //for benchmarking
    /*
    if (a_DEBUG)
    {
        _total++;
        if (_total % 1000 == 0) printf("%i messages\n", _total);
        return;
    }
    */

    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;

    //determine if message is new, if so send out
    /*
        Client should send last 2 control states each packet, must handle redundant control state properly
    */

    class agent_control_state_StoC m;
    m.id = a->id;
    m.seq = seq;
    m.cs = cs;
    m.theta = theta;
    m.phi = phi;
    m.broadcast();

    a->handle_control_state(seq, cs, theta, phi);

    /*
        Warning: setting agent client id by the client the last control state was received for that agent
        This needs to be done properly, at agent creation
    */
    //A->client_id = client_id;

}

inline void hitscan_entity_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("Agents::Agent not found for client %d. message_id=%d\n", client_id, message_id);
        return;
    }

    ItemType weapon_type = Toolbelt::get_agent_selected_item_type(a->id);

    if (type == ENTITY_AGENT)
    {
        Agents::Agent* agent = Agents::get_agent((AgentID)this->id);
        if (agent == NULL || agent->vox == NULL) return;
        force_update_agent_vox(a);
        // apply damage
        const int player_damage = 5;
        agent->status.apply_damage(player_damage, a->id, a->type);
    }
    else
    {
        class Entities::Entity* obj = Entities::get(EntityType(type), EntityID(id));
        if (obj == NULL) return;

        // apply damage
        int obj_dmg = Item::get_item_object_damage(weapon_type);
        obj_dmg = obj_dmg + roundf(float(obj_dmg) * this->charge_progress);
        auto health = GET_COMPONENT_INTERFACE(Health, obj);
        if (health != NULL)
            health->take_damage(obj_dmg);

        // knockback
        if (Item::does_knockback(weapon_type))
        {
            auto knockback = GET_COMPONENT_INTERFACE(Knockback, obj);
            if (knockback != NULL)
                knockback->get_hit(a->forward_vector(),
                                   Toolbelt::get_agent_selected_item_type(a->id),
                                   this->charge_progress + 1.0f);
        }

        AgentID* attacker_id = (AgentID*)malloc(sizeof(AgentID));
        *attacker_id = a->id;

        auto state_machine = GET_COMPONENT_INTERFACE(StateMachine, obj);
        if (state_machine != NULL)
            state_machine->receive_event("agent_attacked", (void*)attacker_id);
    }

    agent_hitscan_entity_StoC msg;
    msg.id = a->id;
    msg.target_id = this->id;
    msg.target_type = this->type;
    msg.broadcast();
}

// hitscan target:block
inline void hitscan_block_CtoS::handle()
{
    if (charge_progress < 0 || charge_progress > 1) return;
    Agents::Agent* a = NetServer::agents[client_id];
    IF_ASSERT(a == NULL) return;

    if (!is_valid_z(this->position)) return;
    Vec3i position = translate_position(this->position);

    agent_hitscan_block_StoC msg;
    msg.id = a->id;
    msg.position = position;
    msg.broadcast();

    if (position.z <= 0) return; // dont damage floor
    CubeType cube_type = t_map::get(position);

    ItemType weapon_type = Toolbelt::get_agent_selected_item_type(a->id);
    int weapon_block_damage = Item::get_item_block_damage(weapon_type, cube_type);
    if (weapon_block_damage <= 0) return;
    weapon_block_damage += int(float(weapon_block_damage) * this->charge_progress);

    // TODO -- load this from dat
    //TerrainModificationAction tma = Item::get_item_terrain_modification_enum((ItemType)weapon_type);
    t_map::apply_damage_broadcast(position, weapon_block_damage, TMA_PICK);
}

inline void hitscan_mech_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    IF_ASSERT(a == NULL) return;

    // send generic shoot packet
    agent_hitscan_nothing_StoC msg;
    msg.id = a->id;
    msg.broadcast();

    t_mech::hit_mech(mech_id);
}

inline void hitscan_none_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    IF_ASSERT(a == NULL) return;

    agent_hitscan_nothing_StoC msg;
    msg.id = a->id;
    msg.broadcast();
}

inline void launch_projectile_CtoS::handle()
{
    IF_ASSERT(charge_progress < 0 || charge_progress > 1) return;
    Agents::Agent* a = NetServer::agents[client_id];
    IF_ASSERT(a == NULL) return;
    agent_launched_projectile_StoC msg;
    msg.id = a->id;
    msg.item_type = this->item_type;
    msg.broadcast();

    ItemType item_type = ItemType(this->item_type);

    // TODO load from dat later, and modify by charged status
    //float force = Item::get_projectile_force(item_type);
    //force *= 1.0f + this->charge_progress;
    static const float PLAYER_ARM_FORCE = 15.0f;
    float force = PLAYER_ARM_FORCE * (1.0f + this->charge_progress);
    Vec3 n = vec3_normalize(this->velocity);
    n = vec3_scalar_mult(n, force);

    // TODO -- use some kind of configuration to pick this up
    // Needs to support a physics interface, owner property, and networking
    if (strcmp(Item::get_item_name(item_type), "plasma_grenade") == 0)
    {
        Particle::Grenade* g = Particle::grenade_list->create();
        if (g == NULL) return;
        g->set_state(this->position, n);
        g->owner = a->id;
        g->broadcast();
    }
    else
    {
        printf("Warning: unhandled projectile type %d\n", item_type);
    }
}

inline void agent_set_block_CtoS::handle()
{
    IF_ASSERT(!is_valid_z(this->position)) return;
    if (this->position.z == 0) return;

    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL || a->status.dead) return;

    // fire block applier
    Item::Item* placer = Item::get_item((ItemID)placer_id);
    if (placer == NULL) return;
    Item::ItemAttribute* attr = Item::get_item_attributes(placer->type);
    IF_ASSERT(attr == NULL) return;
    CubeType cube_type = attr->cube_type;

    // do block place checks here later
    // problem is, fire/(decrement ammo) packet is separate, and isnt aware of this failure

    Vec3i position = translate_position(this->position);

    // dont set on existing block
    if (!t_map::block_can_be_placed(position, cube_type)) return;

    // check this player first, most likely to be colliding
    bool collides = false;
    t_map::set_fast(position, cube_type); // set temporarily to test against
    if (agent_collides_terrain(a))
        collides = true;
    else
    {
        for (size_t i=0; i<Agents::agent_list->max; i++)
        {
            Agents::Agent* agent = &Agents::agent_list->objects[i];
            if (agent->id == Agents::agent_list->null_id || agent == a) continue;
            if (agent_collides_terrain(agent))
            {
                collides = true;
                break;
            }
        }
    }
    t_map::set_fast(position, EMPTY_CUBE);  // unset

    if (collides) return;

    Toolbelt::use_block_placer(a->id, (ItemID)placer_id);
    t_map::broadcast_set_block(position, cube_type);
    agent_placed_block_StoC msg;
    msg.id = a->id;
    msg.broadcast();

    t_map::broadcast_set_block_action(position, cube_type, TMA_PLACE_BLOCK);

    /*
        Handle Special Block Placement
        TODO -- move this to t_map's handler
    */
    static int _control_node = t_map::get_cube_type("control_node");
    if (cube_type == _control_node)
        t_map::add_control_node(position);
}

static Entities::Entity* place_object_handler(EntityType type, Vec3i position, AgentID owner_id)
{
    if (Entities::point_occupied_by_type(ENTITY_TURRET, position)) return NULL;
    if (Entities::point_occupied_by_type(ENTITY_AGENT_SPAWNER, position)) return NULL;

    // zip down
    static const int ITEM_PLACEMENT_Z_DIFF_LIMIT = 3;

    using Entities::Entity;
    Entity* entity = Entities::create(type);
    if (entity == NULL) return NULL;

    int height = 1;
    auto dims = GET_COMPONENT_INTERFACE(Dimension, entity);
    if (dims != NULL)
        height = dims->get_integer_height();

    int z = t_map::get_highest_open_block(position, height);
    if (position.z - z > ITEM_PLACEMENT_Z_DIFF_LIMIT || position.z - z < 0)
    {
        Entities::destroy(entity);
        return NULL;
    }

    position.z = z;
    if (Entities::point_occupied_by_type(ENTITY_TURRET, position) ||
        Entities::point_occupied_by_type(ENTITY_AGENT_SPAWNER, position))
    {
        Entities::destroy(entity);
        return NULL;
    }

    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    if (physics != NULL)
    {
        Vec3 p = vec3_add(vec3_init(position), vec3_init(0.5, 0.5f, 0.0f));
        physics->set_position(p);
    }

    auto owner = GET_COMPONENT_INTERFACE(Owner, entity);
    if (owner != NULL) owner->set_owner(owner_id);

    return entity;
}

inline void place_spawner_CtoS::handle()
{
    EntityType type = ENTITY_AGENT_SPAWNER;
    Agents::Agent* a = NetServer::agents[client_id];
    IF_ASSERT(a == NULL) return;
    IF_ASSERT(!is_valid_z(this->position)) return;
    if (this->position.z == 0) return;
    Entities::Entity* obj = place_object_handler(type, position, a->id);
    if (obj == NULL) return;
    Entities::ready(obj);
}

inline void place_turret_CtoS::handle()
{
    EntityType type = ENTITY_TURRET;
    Agents::Agent* a = NetServer::agents[client_id];
    IF_ASSERT(a == NULL) return;
    IF_ASSERT(!is_valid_z(this->position)) return;
    if (this->position.z == 0) return;
    Entities::Entity* obj = place_object_handler(type, position, a->id);
    if (obj == NULL) return;
    Entities::ready(obj);
}

inline void choose_spawner_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[this->client_id];
    IF_ASSERT(a == NULL) return;
    EntityID spawner_id = EntityID(this->spawner_id);
    IF_ASSERT(!isValid(spawner_id)) return;
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
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    a->set_camera_state(x,y,z, theta,phi);
    a->camera_ready = true;
}

inline void teleport_me_CtoS::handle()
{
    #if PRODUCTION
    GS_ASSERT(false);
    return;
    #endif

    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    a->teleport(this->position);
}

inline void gohome_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    a->spawn_state();
}

#endif
