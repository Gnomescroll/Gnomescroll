#include "targeting.hpp"

#include <c_lib/ray_trace/hitscan.hpp>
#include <c_lib/ray_trace/handlers.hpp>

namespace Components
{

/* Targeting */

Agent_state* TargetingComponent::acquire_target(
    int id, ObjectType type, int team, float camera_z, Vec3 position, Vec3* firing_direction
) {    
    // lock on agent
    position.z = camera_z;
    Agent_state* agent = Hitscan::lock_agent_target(
        position, firing_direction, team,
        this->sight_range, this->target_acquisition_probability,
        this->attack_enemies, this->attack_random
    );
    return agent;
}

Agent_state* TargetingComponent::fire_on_target(
    int id, ObjectType type, int team, float camera_z, Vec3 position
) {    
    // lock on agent
    position.z = camera_z;
    Vec3 firing_direction;
    Agent_state* agent = this->acquire_target(
        id, type, team, camera_z, position, &firing_direction
    );
    if (agent == NULL) return NULL;

    // normalize and bias vector
    normalize_vector(&firing_direction);
    if (this->accuracy_bias)   // apply bias
        firing_direction = vec3_bias_random(firing_direction, this->accuracy_bias);

    // get target
    Hitscan::HitscanTarget t = Hitscan::shoot_at_agent(
        position, firing_direction, id, type,
        agent, this->sight_range
    );

    // let handle target hit based on attacker properties
    Hitscan::handle_hitscan_target(t, this->attacker_properties);

    // send firing packet
    Hitscan::broadcast_object_fired(id, type, t);

    // apply custom handling
    // play sounds
    // play animations

    return agent;
}

Agent_state* TargetingComponent::fire_on_known_target(
    int id, ObjectType type, float camera_z, Vec3 position, Vec3 direction, Agent_state* agent
) {    
    // lock on agent
    position.z = camera_z;

    // normalize and bias vector
    normalize_vector(&direction);
    if (this->accuracy_bias)   // apply bias
        direction = vec3_bias_random(direction, this->accuracy_bias);

    // get target
    Hitscan::HitscanTarget t = Hitscan::shoot_at_agent(
        position, direction, id, type,
        agent, this->sight_range
    );

    // let handle target hit based on attacker properties
    Hitscan::handle_hitscan_target(t, this->attacker_properties);

    // send firing packet
    Hitscan::broadcast_object_fired(id, type, t);

    // apply custom handling
    // play sounds
    // play animations

    return agent;
}


} // Objects
