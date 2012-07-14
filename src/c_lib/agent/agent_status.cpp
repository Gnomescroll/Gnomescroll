#include "agent_status.hpp"

#include <math.h>

#if DC_SERVER
#include <chat/server.hpp>
#include <chat/interface.hpp>
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
vox_crouched(false),
lifetime(0),
inventory(NULL),
toolbelt(NULL)
{
    strcpy(this->name, AGENT_UNDEFINED_NAME);
}

Agent_status::~Agent_status()
{
}

void Agent_status::set_spawner(int pt)
{
    using Components::agent_spawner_component_list;
    using Components::BASE_SPAWN_ID;
    
    if (pt != BASE_SPAWN_ID)
    {   // check new spawner exists
        GS_ASSERT(false);
        // TODO -- reimplement spawners without teams
        //if (!agent_spawner_component_list->spawner_exists(this->team, pt))
        //{
            //if (agent_spawner_component_list->spawner_exists(this->team, this->spawner))
                //return;     // current spawner valid, leave it
            //else
                //pt = BASE_SPAWN_ID; // current spawner invalid, default to base
        //}
    }
    this->spawner = pt;
    #if DC_SERVER
    spawn_location_StoC msg;
    msg.pt = pt;
    msg.sendToClient(this->a->id);
    #endif
}

void Agent_status::set_spawner()
{
    GS_ASSERT(false);
    // TODO -- reimplement spawners without teams
    //int pt = Components::agent_spawner_component_list->get_random_spawner(this->team);
    //this->spawner = pt;
    //#if DC_SERVER
    //spawn_location_StoC msg;
    //msg.pt = pt;
    //msg.sendToClient(this->a->id);
    //#endif
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
    
    if (!dmg) return this->health;
    if (this->health <= 0) return this->health;

    agent_damage_StoC dmg_msg;
    dmg_msg.id = a->id;
    dmg_msg.dmg = dmg;
    dmg_msg.broadcast();
    
    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health;

    this->send_health_msg();
    
    return this->health;
}

void Agent_status::send_health_msg()
{
    agent_health_StoC health_msg;
    health_msg.id = a->id;
    health_msg.health = this->health;
    health_msg.broadcast();
}

int Agent_status::apply_damage(int dmg, int inflictor_id, ObjectType inflictor_type, int part_id)
{
    #if DC_SERVER
    // dont allow player kills
    if ((inflictor_type == OBJECT_AGENT || inflictor_type == OBJECT_GRENADE)
      && inflictor_id != this->a->id) return this->health;
    
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
            dmg = randrange(15,25);
            break;
        case AGENT_PART_TORSO:
            dmg = randrange(10,15);
            break;
        case AGENT_PART_LARM:
            dmg = randrange(5,10);
            break;
        case AGENT_PART_RARM:
            dmg = randrange(5,10);
            break;
        case AGENT_PART_LLEG:
            dmg = randrange(5,10);
            break;
        case AGENT_PART_RLEG:
            dmg = randrange(5,10);
            break;
        default:
            printf("WARNING Agent_status::apply_hitscan_laser_damage_to_part -- unknown part %d\n", part_id);
            break;
    }
    
    return this->apply_damage(dmg, inflictor_id, inflictor_type, part_id);
}

bool Agent_status::die()
{
    if (this->dead) return false;
    this->dead = true;
    this->deaths++;

    #if DC_SERVER
    AgentDeaths_StoC deaths_msg;
    deaths_msg.id = this->a->id;
    deaths_msg.deaths = this->deaths;
    deaths_msg.broadcast();
    
    agent_dead_StoC dead_msg;
    dead_msg.id = this->a->id;
    dead_msg.dead = this->dead;
    dead_msg.broadcast();

    Toolbelt::agent_died(this->a->id);
    ItemContainer::agent_died(this->a->id);
    #endif

    return true;
}

bool Agent_status::die(int inflictor_id, ObjectType inflictor_type, AgentDeathMethod death_method)
{
    bool killed = this->die();
    if (!killed) return false;
    
    Agent_state* attacker;
    //Turret* turret;
	switch (inflictor_type)
	{
		case OBJECT_GRENADE:
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
	// send conflict notification to clients
	agent_conflict_notification_StoC msg;
	//Turret* turret;
	switch (inflictor_type)
	{
		case OBJECT_GRENADE:
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
	
    return true;
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
    }
}

void Agent_status::kill_slime()
{
    this->slime_kills++;
}

void Agent_status::send_scores(int client_id)
{
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
void Agent_status::send_scores()
{
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

#if DC_SERVER
void Agent_status::set_fresh_state()
{
    a->spawn_state();
    this->lifetime = 0;
    this->restore_health();
    
    // revive
    dead = false;
    agent_dead_StoC dead_msg;
    dead_msg.id = a->id;
    dead_msg.dead = dead;
    dead_msg.broadcast();

    ItemContainer::agent_born(this->a->id);
}

void Agent_status::respawn()
{
    if (!dead) return;  // ignore if not waiting to respawn
    
    respawn_countdown--;                  // decrement
    if (respawn_countdown > 0) return;  // abort if not ready
    this->set_fresh_state();
    respawn_countdown = RESPAWN_TICKS; // reset timer
}
#endif

float Agent_status::get_spawn_angle()
{
    return 0.5f;
}

void Agent_status::restore_health()
{
    if (this->health == AGENT_HEALTH) return;
    this->health = AGENT_HEALTH;
    this->send_health_msg();
}

void Agent_status::at_base()
{
    this->restore_health();
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

        default:
            break;
    }
    return can;
}

#if DC_SERVER
bool Agent_status::consume_item(ItemID item_id)
{
    int item_type = Item::get_item_type(item_id);
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    if (item_type == NULL_ITEM_TYPE) return false;
    
    ItemGroup item_group = Item::get_item_group_for_type(item_type);
    GS_ASSERT(item_group == IG_CONSUMABLE);
    if (item_group != IG_CONSUMABLE) return false;

    Item::ItemAttribute* attr = Item::get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return false;

    static const int repair_kit = Item::get_item_type("repair_kit");

    if (item_type == repair_kit)
    {
        if (this->health >= (int)health_max) return false;
        GS_ASSERT(attr->repair_agent_amount > 0);
        this->heal(attr->repair_agent_amount);
        this->send_health_msg();
    }
    else
    {
        GS_ASSERT(false);
        return false;
    }
    return true;
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
