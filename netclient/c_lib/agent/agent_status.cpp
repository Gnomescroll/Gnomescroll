#include "agent_status.hpp"

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
    if (!dead) {
        dead = true;
        agent_dead_StoC* dead_msg = new agent_dead_StoC(a->id);
        dead_msg->broadcast();
        a->teleport(a->s.x, a->s.y, a->s.z + 10.0f);
    }
}

//void Agent_status::respawn() {
    
//}
