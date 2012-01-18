#include "agent_status.hpp"

#include <math.h>

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

int Agent_status::apply_damage(int dmg, int inflictor_id, Object_types inflictor_type) {
    int res = this->apply_damage(dmg);
    if (!this->health) die(inflictor_id, inflictor_type);
    return res;
}

int Agent_status::die() {
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

int Agent_status::die(int inflictor_id, Object_types inflictor_type) {
    int killed = die();
    Agent_state* agent;
    if (killed) {
        switch (inflictor_type) {
            case OBJ_TYPE_AGENT:
                agent = STATE::agent_list.get(inflictor_id);
                if (agent != NULL) {
                    agent->status.kill(a->id);
                }
                break;
            case OBJ_TYPE_SLIME:
                //Monsters::Slime* slime = STATE::slime_list.get(inflictor_id);
                //if (slime != NULL) {}
                break;
            default:
                printf("Agent_state::die -- OBJ_TYPE %d not handled\n", inflictor_type);
                break;
        }
    }
    return killed;
}

void Agent_status::kill(int victim_id) {
    if (victim_id == a->id) {
        suicides++;
        AgentSuicides_StoC as;
        as.id = a->id;
        as.suicides = suicides;
        as.broadcast();
    } else {
        kills++;
        AgentKills_StoC ak;
        ak.id = a->id;
        ak.kills = kills;
        ak.broadcast();
    }
}

int Agent_status::score() {
    return kills - suicides;
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
    health = AGENT_HEALTH;
    agent_health_StoC health_msg;
    health_msg.id = a->id;
    health_msg.health = health;
    health_msg.sendToClient(a->client_id);

    // revive
    dead = false;
    agent_dead_StoC dead_msg;
    dead_msg.id = a->id;
    dead_msg.dead = dead;
    dead_msg.broadcast();

    respawn_countdown = RESPAWN_TICKS; // reset timer
}
