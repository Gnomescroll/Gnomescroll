#include "agent_status.hpp"

#include <math.h>
#include <c_lib/common/random.h>

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
    if (dead) return 0;
    
    health -= dmg;
    health = (health < 0) ? 0 : health;

    agent_health_StoC* health_msg = new agent_health_StoC(a->id, health);
    health_msg->sendToClient(a->client_id);

    if (!health) die();
    
    return health;
}

void Agent_status::die() {
    if (dead) return;
    dead = true;
    agent_dead_StoC* dead_msg = new agent_dead_StoC(a->id, dead);
    dead_msg->broadcast();
}

void Agent_status::get_spawn_point(int* spawn) {
    // generate a spawn point
    //spawn[0] = randrange(0, xmax);
    //spawn[1] = randrange(0, ymax);
    spawn[0] = randrange(0, 128); // use actual map sizes!
    spawn[1] = randrange(0, 128);
    spawn[2] = _get_highest_open_block(spawn[0], spawn[1], (int)ceil(a->box.b_height));
    printf("Respawning at: %d %d %d\n", spawn[0], spawn[1], spawn[2]);
}

void Agent_status::respawn() {
    if (!dead) return;  // ignore if not waiting to respawn
    
    respawn_countdown--;                  // decrement
    if (respawn_countdown > 0) return;  // abort if not ready
    
    // update position
    int spawn[3];
    get_spawn_point(spawn);
    a->teleport(spawn[0], spawn[1], spawn[2]);

    // restore health
    health = AGENT_HEALTH;
    agent_health_StoC* health_msg = new agent_health_StoC(a->id, health);
    health_msg->sendToClient(a->client_id);

    // revive
    dead = false;
    agent_dead_StoC* dead_msg = new agent_dead_StoC(a->id, dead);
    dead_msg->broadcast();

    respawn_countdown = RESPAWN_TICKS; // reset timer
}
