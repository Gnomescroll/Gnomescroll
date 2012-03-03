#include "agent_status.hpp"

#include <math.h>

#ifdef DC_SERVER
#include <chat/server.hpp>
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

const char AGENT_UNDEFINED_NAME[] = "undefined-agent-name";

Agent_status::Agent_status(Agent_state* a)
:
a(a),
health(AGENT_HEALTH),
dead(false),
respawn_countdown(RESPAWN_TICKS),
spawner(BASE_SPAWN_ID),  // -1 will mean default spawn point (base)
kills(0),
deaths(0),
suicides(0),
health_max(AGENT_HEALTH),
identified(false),
team(0),
has_flag(false),
flag_captures(0),
coins(0),
vox_crouched(false)
{
    strcpy(this->name, AGENT_UNDEFINED_NAME);
}

void Agent_status::set_spawner(int pt)
{
    this->spawner = pt;
}

void Agent_status::set_spawner()
{
    int pt = STATE::spawner_list->get_random_spawner(this->team);
    this->spawner = pt;
}

bool Agent_status::set_name(char* name)
{
    #ifdef DC_SERVER
    if (strcmp(AGENT_UNDEFINED_NAME, name) == 0)
        return false;
    if (name[0] == '\0')
        return false;
    #endif
    
    if (strlen(name) > PLAYER_NAME_MAX_LENGTH)
        name[PLAYER_NAME_MAX_LENGTH-1] = '\0';

    bool new_name = (strcmp(this->name, name) == 0) ? false : true;

    strcpy(this->name, name);
    #ifdef DC_SERVER
    agent_name_StoC msg;
    msg.id = this->a->id;
    strcpy(msg.name, this->name);
    msg.broadcast();
    #endif

    return new_name;
}

int Agent_status::apply_damage(int dmg) {
    if (dead) return this->health;
    
    // forward dmg indicator packet
    agent_damage_StoC dmg_msg;
    
    dmg_msg.id = a->id;
    dmg_msg.dmg = dmg;
    dmg_msg.broadcast();

    if (!dmg) return this->health;

    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health;

    agent_health_StoC health_msg;
    health_msg.id = a->id;
    health_msg.health = this->health;
    health_msg.sendToClient(a->client_id);

    return this->health;
}

int Agent_status::apply_damage(int dmg, int inflictor_id, Object_types inflictor_type, int part_id)
{
    #ifdef DC_SERVER
    // dont allow team kills
    if (inflictor_type == OBJ_TYPE_AGENT && inflictor_id != this->a->id)
    {
        Agent_state *inf = STATE::agent_list->get(inflictor_id);
        if (inf == NULL) return this->health;
        if (inf->status.team == this->team && (!Options::team_kills)) return this->health;
    }
    
    int health = this->apply_damage(dmg);
    AgentDeathMethod death_method = DEATH_NORMAL;
    if (part_id == AGENT_PART_HEAD)
        death_method = DEATH_HEADSHOT;
    else if (inflictor_type == OBJ_TYPE_GRENADE)
        death_method = DEATH_GRENADE;
        
    if (!this->health) die(inflictor_id, inflictor_type, death_method);
    #endif
    return health;
}

int Agent_status::apply_hitscan_laser_damage_to_part(int part_id, int inflictor_id, Object_types inflictor_type)
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
    if (dead) return 0;
    dead = true;
    deaths++;

    AgentDeaths_StoC deaths_msg;
    deaths_msg.id = a->id;
    deaths_msg.deaths = deaths;
    deaths_msg.broadcast();
    
    agent_dead_StoC dead_msg;
    dead_msg.id = a->id;
    dead_msg.dead = dead;
    dead_msg.broadcast();
    return 1;
}

int Agent_status::die(int inflictor_id, Object_types inflictor_type, AgentDeathMethod death_method)
{
    if (inflictor_type == OBJ_TYPE_GRENADE)
        inflictor_type = OBJ_TYPE_AGENT;
        
    int killed = this->die();
    Agent_state* attacker;
    if (killed) {
        switch (inflictor_type) {
            case OBJ_TYPE_AGENT:
                attacker = STATE::agent_list->get(inflictor_id);
                if (attacker != NULL) {
                    attacker->status.kill(this->a->id);
                }
                break;
            case OBJ_TYPE_SLIME:
                //Monsters::Slime* slime = STATE::slime_list->get(inflictor_id);
                //if (slime != NULL) {}
                break;
            default:
                printf("Agent_state::die -- OBJ_TYPE %d not handled\n", inflictor_type);
                break;
        }

        #ifdef DC_SERVER
        // drop any items (FLAG)
        if (this->has_flag) {
            this->drop_flag();
            ServerState::ctf->agent_drop_flag(this->team, this->a->s.x, this->a->s.y, this->a->s.z);
        }

        // send conflict notification to clients
        if (inflictor_type == OBJ_TYPE_AGENT)
        {
            agent_conflict_notification_StoC msg;
            msg.victim = this->a->id;
            msg.attacker = inflictor_id;
            msg.method = death_method;    // put headshot, grenades here
            msg.broadcast();
        }
        #endif

    }
    return killed;
}

void Agent_status::kill(int victim_id) {
    if (victim_id == this->a->id) {
        suicides++;
        AgentSuicides_StoC as;
        as.id = this->a->id;
        as.suicides = suicides;
        as.broadcast();
    } else {
        kills++;
        AgentKills_StoC ak;
        ak.id = this->a->id;
        ak.kills = kills;
        ak.broadcast();
    }
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
    this->health = AGENT_HEALTH;
    agent_health_StoC health_msg;
    health_msg.id = a->id;
    health_msg.health = this->health;
    health_msg.sendToClient(a->client_id);
}

void Agent_status::pickup_flag() {
    if (!this->has_flag) {
        AgentPickupFlag_StoC msg;
        msg.id = this->a->id;
        msg.broadcast();
    }
    
    this->has_flag = true;
}
void Agent_status::drop_flag() {
    if (this->has_flag) {
        AgentDropFlag_StoC msg;
        msg.id = this->a->id;
        msg.broadcast();
    }

    this->has_flag = false;
}
void Agent_status::score_flag() {
    if (this->has_flag) {
        AgentScoreFlag_StoC msg;
        msg.id = this->a->id;
        msg.broadcast();

        this->flag_captures++;
    }
    const unsigned int coins = get_object_cost(OBJ_TYPE_SPAWNER) * 3;
    this->add_coins(coins);
    this->has_flag = false;
}

void Agent_status::set_team(int team)
{
    if (team == this->team) return;

    #ifdef DC_SERVER
    chat_server->player_join_team(this->a->id, this->team, team);
    #endif
    
    //printf("Agent_Status:: set_team.  agent=%d team=%d\n", this->a->id, team);
    // respawn instantly if switching from viewer to team
    if (this->team == 0 && team)
        this->respawn_countdown = 0;
    this->team = team;

    #ifdef DC_SERVER
    this->set_spawner();    // choose new spawn point

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
    #ifdef DC_SERVER
    this->send_coin_packet();
    #endif
}

void Agent_status::spend_coins(unsigned int coins)
{
    if (coins==0) return;
    this->coins -= coins;
    #ifdef DC_SERVER
    this->send_coin_packet();
    #endif
}

void Agent_status::send_coin_packet()
{
    #ifdef DC_SERVER
    agent_coins_StoC msg;
    msg.coins = this->coins;
    msg.sendToClient(this->a->id);
    #endif
}

bool Agent_status::can_afford(Object_types obj)
{
    unsigned int cost = get_object_cost(obj);
    return can_afford(cost);
}

bool Agent_status::can_afford(unsigned int coins)
{
    return (coins <= this->coins);
}

bool Agent_status::purchase(Object_types obj)
{
    unsigned int cost = get_object_cost(obj);
    if (!this->can_afford(cost)) return false;
    this->spend_coins(cost);
    return true;
}

