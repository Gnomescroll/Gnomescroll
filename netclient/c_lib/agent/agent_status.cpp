#include "agent_status.hpp"

#include <math.h>

//#include <c_lib/items/inventory/include.hpp>

#if DC_SERVER
#include <chat/server.hpp>
#include <c_lib/chat/interface.hpp>
#endif

/*
 * Agent_status has miscellaneous status properties (health, dead, ...)
 * Its methods are to be used by the server ONLY
 * The reflexive methods that will be called in the client are stored on Agent_event
 *
 * e.g.
 * Agent_status::die() // in server
 * Agent_event::died() // in client
 *
 */

const int VOXEL_MODEL_RESTORE_WAIT = 30 * 10; // ~ once every 10 seconds

const char AGENT_UNDEFINED_NAME[] = "undefined-agent-name";

Agent_status::Agent_status(Agent_state* a)
:
a(a),
voxel_model_restore_throttle(0),
health(AGENT_HEALTH),
dead(false),
respawn_countdown(RESPAWN_TICKS),
spawner(Components::BASE_SPAWN_ID),  // -1 will mean default spawn point (base)
kills(0),
deaths(0),
suicides(0),
slime_kills(0),
health_max(AGENT_HEALTH),
owned_spawners(0),
owned_turrets(0),
identified(false),
team(0),
has_flag(false),
flag_captures(0),
coins(0),
vox_crouched(false),
base_restore_rate_limiter(0),
lifetime(0),
inventory(NULL),
toolbelt(NULL)
{
    strcpy(this->name, AGENT_UNDEFINED_NAME);
    #if DC_SERVER
    this->load_inventories();
    #endif
}

Agent_status::~Agent_status()
{
    #if DC_SERVER
    assert(this->inventory != NULL);
    Items::destroy_inventory(this->inventory);
    assert(this->toolbelt != NULL);
    Items::destroy_inventory(this->toolbelt);
    #endif
}

#if DC_SERVER
void Agent_status::load_inventories()
{   // get inventories from inventory repo
    this->inventory = Items::create_inventory(OBJECT_AGENT_INVENTORY);
    assert(this->inventory != NULL);
    this->inventory->owner = this->a->id;

    // TODO
    this->inventory->add_action(0, OBJECT_GEMSTONE_AMETHYST, 1, 0);
    
    this->toolbelt = Items::create_inventory(OBJECT_AGENT_TOOLBELT);
    assert(this->toolbelt != NULL);
    this->toolbelt->owner = this->a->id;
    this->toolbelt->add_action(1, OBJECT_GEMSTONE_AMETHYST, 1, 5);
    
}

void Agent_status::send_inventories_to_client()
{
    assert(this->inventory != NULL);
    this->inventory->sendToClientCreate(this->a->id);
    assert(this->toolbelt != NULL);
    this->toolbelt->sendToClientCreate(this->a->id);
}
#endif

void Agent_status::set_spawner(int pt)
{
    using Components::agent_spawner_component_list;
    using Components::BASE_SPAWN_ID;
    
    if (pt != BASE_SPAWN_ID)
    {   // check new spawner exists
        if (!agent_spawner_component_list->spawner_exists(this->team, pt))
        {
            if (agent_spawner_component_list->spawner_exists(this->team, this->spawner))
                return;     // current spawner valid, leave it
            else
                pt = BASE_SPAWN_ID; // current spawner invalid, default to base
        }
    }
    //printf("Setting spawner to %d\n", pt);
    this->spawner = pt;
    #if DC_SERVER
    spawn_location_StoC msg;
    msg.pt = pt;
    msg.sendToClient(this->a->id);
    #endif
}

void Agent_status::set_spawner()
{
    int pt = Components::agent_spawner_component_list->get_random_spawner(this->team);
    this->spawner = pt;
    #if DC_SERVER
    spawn_location_StoC msg;
    msg.pt = pt;
    msg.sendToClient(this->a->id);
    #endif
}

bool Agent_status::set_name(char* name)
{
    #if DC_SERVER
    if (strcmp(AGENT_UNDEFINED_NAME, name) == 0)    // cant be the undefined holder
        return false;
    if (name[0] == '\0')                            // no empties
        return false;
    #endif
    
    if (strlen(name) > PLAYER_NAME_MAX_LENGTH)
        name[PLAYER_NAME_MAX_LENGTH-1] = '\0';

    bool new_name = (strcmp(this->name, name) == 0) ? false : true;

    strcpy(this->name, name);
    #if DC_SERVER
    agent_name_StoC msg;
    msg.id = this->a->id;
    strcpy(msg.name, this->name);
    msg.broadcast();
    #endif

    return new_name;
}

void Agent_status::check_missing_name()
{
    #if DC_CLIENT
    if (strcmp(name, AGENT_UNDEFINED_NAME) == 0)
    {
        request_agent_name_CtoS msg;
        msg.id = this->a->id;
        msg.send();
    }
    #endif
}

void Agent_status::heal(unsigned int amt)
{
    if (this->dead) return;
    this->health += amt;
    if (this->health > (int)this->health_max)
        this->health = this->health_max;
}

int Agent_status::apply_damage(int dmg)
{
    if (this->dead) return this->health;
    
    // forward dmg indicator packet
    agent_damage_StoC dmg_msg;
    
    dmg_msg.id = a->id;
    dmg_msg.dmg = dmg;
    dmg_msg.broadcast();

    if (!dmg) return this->health;
    if (this->health <= 0) return this->health;
    
    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health;

    agent_health_StoC health_msg;
    health_msg.id = a->id;
    health_msg.health = this->health;
    health_msg.sendToClient(a->client_id);

    return this->health;
}

int Agent_status::apply_damage(int dmg, int inflictor_id, ObjectType inflictor_type, int part_id)
{
    #if DC_SERVER
    // dont allow team kills
    if ((inflictor_type == OBJECT_AGENT || inflictor_type == OBJECT_GRENADE)
      && inflictor_id != this->a->id)
    {
        Agent_state *inf = STATE::agent_list->get(inflictor_id);
        if (inf == NULL) return this->health;
        if (inf->status.team == this->team && (!Options::team_kills)) return this->health;
    }
    
    int health = this->apply_damage(dmg);
    AgentDeathMethod death_method = DEATH_NORMAL;
    if (part_id == AGENT_PART_HEAD)
        death_method = DEATH_HEADSHOT;
    else if (inflictor_type == OBJECT_GRENADE)
        death_method = DEATH_GRENADE;
    else if (inflictor_type == OBJECT_TURRET)
        death_method = DEATH_TURRET;
        
    if (!this->health) die(inflictor_id, inflictor_type, death_method);
    #endif
    return health;
}

int Agent_status::apply_hitscan_laser_damage_to_part(int part_id, int inflictor_id, ObjectType inflictor_type)
{
    int dmg = 0;

    switch (part_id)
    {
        case AGENT_PART_HEAD:
            dmg = AGENT_PART_HEAD_DAMAGE;
            break;
        case AGENT_PART_TORSO:
            dmg = AGENT_PART_TORSO_DAMAGE;
            break;
        case AGENT_PART_LARM:
            dmg = AGENT_PART_LARM_DAMAGE;
            break;
        case AGENT_PART_RARM:
            dmg = AGENT_PART_RARM_DAMAGE;
            break;
        case AGENT_PART_LLEG:
            dmg = AGENT_PART_LLEG_DAMAGE;
            break;
        case AGENT_PART_RLEG:
            dmg = AGENT_PART_RLEG_DAMAGE;
            break;
        default:
            printf("WARNING Agent_status::apply_hitscan_laser_damage_to_part -- unknown part %d\n", part_id);
            break;
    }
    
    return this->apply_damage(dmg, inflictor_id, inflictor_type, part_id);
}

int Agent_status::die()
{
    if (this->dead) return 0;
    dead = true;
    deaths++;

    #if DC_SERVER
    AgentDeaths_StoC deaths_msg;
    deaths_msg.id = a->id;
    deaths_msg.deaths = deaths;
    deaths_msg.broadcast();
    
    agent_dead_StoC dead_msg;
    dead_msg.id = a->id;
    dead_msg.dead = dead;
    dead_msg.broadcast();

    this->inventory->remove_all_action();
    this->toolbelt->remove_all_action();
    #endif

    return 1;
}

int Agent_status::die(int inflictor_id, ObjectType inflictor_type, AgentDeathMethod death_method)
{
    if (inflictor_type == OBJECT_GRENADE)
        inflictor_type = OBJECT_AGENT;
        
    int killed = this->die();
    Agent_state* attacker;
    //Turret* turret;
    if (killed)
    {
        switch (inflictor_type)
        {
            case OBJECT_AGENT:
                attacker = STATE::agent_list->get(inflictor_id);
                if (attacker != NULL)
                    attacker->status.kill(this->a->id);
                break;
            //case OBJECT_MONSTER_BOMB:
                //Monsters::Slime* slime = STATE::slime_list->get(inflictor_id);
                //if (slime != NULL) {}
                //break;
            //case OBJECT_TURRET:
                //turret = (Turret*)STATE::object_list->get(inflictor_type, inflictor_id);
                //if (turret == NULL) break;
                //attacker = STATE::agent_list->get(turret->get_owner());
                //if (attacker != NULL)
                    //attacker->status.kill(this->a->id);
                //break;
            default:
                //printf("Agent_state::die -- OBJECT %d not handled\n", inflictor_type);
                break;
        }

        #if DC_SERVER
        // drop any items (FLAG)
        if (this->has_flag)
        {
            this->drop_flag();
            ServerState::ctf->agent_drop_flag(this->team, this->a->s.x, this->a->s.y, this->a->s.z);
        }

        // send conflict notification to clients
        agent_conflict_notification_StoC msg;
        //Turret* turret;
        switch (inflictor_type)
        {
            case OBJECT_AGENT:
                msg.victim = this->a->id;
                msg.attacker = inflictor_id;
                msg.method = death_method;    // put headshot, grenades here
                msg.broadcast();
                break;

            //case OBJECT_TURRET:
                //// lookup turret object, get owner, this will be the inflictor id
                //turret = (Turret*)ServerState::object_list->get(inflictor_type, inflictor_id);
                //if (turret == NULL) break;
                //inflictor_id = turret->get_owner();
                //msg.victim = this->a->id;
                //msg.attacker = inflictor_id;
                //msg.method = death_method;    // put headshot, grenades here
                //msg.broadcast();
                //break;

            default: break;
        }

        #endif

    }
    return killed;
}

void Agent_status::kill(int victim_id)
{
    if (victim_id == this->a->id)
    {
        suicides++;
        AgentSuicides_StoC as;
        as.id = this->a->id;
        as.suicides = suicides;
        as.broadcast();
    }
    else
    {
        kills++;
        AgentKills_StoC ak;
        ak.id = this->a->id;
        ak.kills = kills;
        ak.broadcast();
        this->add_coins(COINS_PER_AGENT_KILL);
    }
}

void Agent_status::kill_slime()
{
    this->slime_kills++;
    if (this->slime_kills % SLIME_KILLS_PER_COIN == 0)
        this->add_coins(1);
}

int Agent_status::score() {
    return kills - suicides + (flag_captures * 3);
}

void Agent_status::send_scores(int client_id) {
    AgentKills_StoC ak;
    ak.id = a->id;
    ak.kills = kills;
    ak.sendToClient(client_id);
    
    AgentDeaths_StoC ad;
    ad.id = a->id;
    ad.deaths = deaths;
    ad.sendToClient(client_id);

    AgentSuicides_StoC as;
    as.id = a->id;
    as.suicides = suicides;
    as.sendToClient(client_id);
}

// to all
void Agent_status::send_scores() {
    AgentKills_StoC ak;
    ak.id = a->id;
    ak.kills = kills;
    ak.broadcast();
    
    AgentDeaths_StoC ad;
    ad.id = a->id;
    ad.deaths = deaths;
    ad.broadcast();

    AgentSuicides_StoC as;
    as.id = a->id;
    as.suicides = suicides;
    as.broadcast();
}

void Agent_status::respawn() {
    if (!dead) return;  // ignore if not waiting to respawn
    
    respawn_countdown--;                  // decrement
    if (respawn_countdown > 0) return;  // abort if not ready
    
    a->spawn_state();
    this->lifetime = 0;
    // restore health
    this->restore_health();
    // restore ammo
    this->a->weapons.restore_ammo();
    
    // revive
    dead = false;
    agent_dead_StoC dead_msg;
    dead_msg.id = a->id;
    dead_msg.dead = dead;
    dead_msg.broadcast();

    respawn_countdown = RESPAWN_TICKS; // reset timer
}

float Agent_status::get_spawn_angle()
{
    switch (this->team)
    {
        case 1:
            return 0.5f;
        case 2:
            return -0.5f;
        default:
            return 0.0f;
    }
}

void Agent_status::restore_health()
{
    if (this->health == AGENT_HEALTH) return;
    this->health = AGENT_HEALTH;
    agent_health_StoC health_msg;
    health_msg.id = a->id;
    health_msg.health = this->health;
    health_msg.sendToClient(a->client_id);
}

void Agent_status::at_base()
{
    this->base_restore_rate_limiter++;
    if (this->base_restore_rate_limiter % AGENT_BASE_PROXIMITY_EFFECT_RATE != 0) return;
    this->restore_health();
    this->a->weapons.restore_ammo();
}

bool Agent_status::pickup_flag() {
    if (!this->has_flag) {
        AgentPickupFlag_StoC msg;
        msg.id = this->a->id;
        msg.broadcast();
    }
    
    this->has_flag = true;
    return true;
}

bool Agent_status::drop_flag() {
    if (this->has_flag) {
        AgentDropFlag_StoC msg;
        msg.id = this->a->id;
        msg.broadcast();
    }

    this->has_flag = false;
    return true;
}
void Agent_status::score_flag() {
    if (this->has_flag) {
        AgentScoreFlag_StoC msg;
        msg.id = this->a->id;
        msg.broadcast();

        this->flag_captures++;
    }
    const unsigned int coins = get_object_cost(OBJECT_AGENT_SPAWNER) * 3;
    this->add_coins(coins);
    this->has_flag = false;
}

void Agent_status::set_team(int team)
{
    if (team == this->team) return;

    #if DC_SERVER
    // am i leaving old team?
    chat_server->player_join_team(this->a->id, this->team, team);
    #endif
    
    // respawn instantly if switching from viewer to team
    if (this->team == 0 && team)
        this->respawn_countdown = 0;
    this->team = team;

    #if DC_SERVER
    this->set_spawner();    // choose new spawn point
    ServerState::revoke_ownership(this->a->id); // revoke ownership of items

    // kill player
    this->dead = true;
    agent_dead_StoC dead_msg;
    dead_msg.id = a->id;
    dead_msg.dead = dead;
    dead_msg.broadcast();
    #endif
}

void Agent_status::add_coins(unsigned int coins)
{
    if (coins==0) return;
    this->coins += coins;
    #if DC_SERVER
    this->send_coin_packet();
    #endif
}

void Agent_status::spend_coins(unsigned int coins, ObjectType item)
{
    #if DC_SERVER
    if (coins==0) return;
    if (item >= 0 && !can_gain_item(item)) return;
    this->coins -= coins;
    this->send_coin_packet();
    #endif
}

const bool Agent_status::can_gain_item(ObjectType item)
{
    if (this->dead) return false;
    //bool can;
    switch (item)
    {
        case OBJECT_TURRET:
            if (owned_turrets >= AGENT_MAX_TURRETS)
                return false;
            break;
            
        case OBJECT_AGENT_SPAWNER:
            if (owned_spawners >= AGENT_MAX_SPAWNERS)
                return false;
            break;

        //case OBJECT_MEAT:
        //case OBJECT_BLOCK_DROP:
        //case OBJECT_GEMSTONE:
            //can = this->inventory->can_add(item);
            //return can;

        default:
            return true;
    }
    return true;
}

// TODO -- duplicate interface for client side -- should go through event
bool Agent_status::gain_item(int item_id, ObjectType item_type)
{
    bool can = this->can_gain_item(item_type);
    if (!can) return false;
    switch (item_type)
    {
        case OBJECT_TURRET:
            owned_turrets++;
            break;
            
        case OBJECT_AGENT_SPAWNER:
            owned_spawners++;
            break;

        case OBJECT_GRENADE_REFILL:
            this->a->weapons.grenades.add_ammo(10);
            break;
            
        case OBJECT_LASER_REFILL:
            this->a->weapons.laser.add_ammo(20);
            break;
            
        case OBJECT_HEALTH_REFILL:
            this->a->status.heal(50);
            break;

        default:
            break;
    }
    return can;
}

#if DC_SERVER
bool Agent_status::gain_stack_item(ObjectType type, int id, int stack_size)
{
    return this->inventory->add_action(id, type, stack_size);
}
#endif

bool Agent_status::lose_item(ObjectType item)
{
    switch (item)
    {
        case OBJECT_TURRET:
            if (owned_turrets <= 0)
            {
                printf("WARNING -- Agent_status::lose_item -- no turrets to lose. id=%d\n", this->a->id);
                return false;
            }
            owned_turrets--;
            break;
            
        case OBJECT_AGENT_SPAWNER:
            if (owned_spawners <= 0)
            {
                printf("WARNING -- Agent_status::lose_item -- no spawners to lose\n");
                return false;
            }
            owned_spawners--;
            break;
            
        default: break;
    }
    return true;
}

void Agent_status::send_coin_packet()
{
    #if DC_SERVER
    agent_coins_StoC msg;
    msg.coins = this->coins;
    msg.sendToClient(this->a->id);
    #endif
}

bool Agent_status::can_purchase(ObjectType obj)
{
    unsigned int cost = get_object_cost(obj);
    return can_gain_item(obj) && can_purchase(cost);
}

bool Agent_status::can_purchase(unsigned int coins)
{
    return (coins <= this->coins);
}

bool Agent_status::purchase(ObjectType obj)
{
    if (!this->can_purchase(obj)) return false;
    unsigned int cost = get_object_cost(obj);
    this->spend_coins(cost, obj);
    return true;
}

void Agent_status::check_if_at_base()
{
    #if DC_CLIENT
    if (ClientState::ctf != NULL
      && ClientState::ctf->is_at_base(
        this->a->status.team,
        this->a->s.x, this->a->s.y, this->a->s.z
    ))
    {   // regenerate model
        voxel_model_restore_throttle++;
        voxel_model_restore_throttle %= VOXEL_MODEL_RESTORE_WAIT;
        if (voxel_model_restore_throttle == 0)
        {
            if (this->a->vox != NULL)
                this->a->vox->restore(this->a->status.team);
        }
    }
    #endif
}

void Agent_status::tick()
{
    if (this->dead)
        this->lifetime = 0;
    else
        this->lifetime++;
}

void switch_agent_ownership(int item_id, ObjectType item_type, int owner, int new_owner)
{
    Agent_state* a;
    if (owner != NO_AGENT)
    {
        a = STATE::agent_list->get(owner);
        if (a != NULL)
            a->status.lose_item(item_type);
    }
    if (new_owner != NO_AGENT)
    {
        a = STATE::agent_list->get(new_owner);
        if (a != NULL)
            a->status.gain_item(item_id, item_type);
    }
}
