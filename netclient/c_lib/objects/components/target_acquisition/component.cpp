#include "component.hpp"

/* TargetAcquisition */

Agent_state* TargetAcquisitionComponent::acquire_target(
    int id, Object_types type, int team, float camera_z, Vec3 position,
    float accuracy_bias, float sight_range,
    bool attack_enemies, bool attack_random,
    Vec3* firing_direction
) {    
    // lock on agent
    position.z = camera_z;
    Agent_state* agent = Hitscan::lock_agent_target(
        position, firing_direction, team,
        sight_range, this->target_acquisition_probability,
        attack_enemies, attack_random
    );
    return agent;
}

Agent_state* TargetAcquisitionComponent::fire_on_target(
    int id, Object_types type, int team, float camera_z, Vec3 position,
    float accuracy_bias, float sight_range,
    bool attack_enemies, bool attack_random
) {    
    // lock on agent
    position.z = camera_z;
    Vec3 firing_direction;
    Agent_state* agent = this->acquire_target(
        id, type, team, camera_z, position, accuracy_bias, sight_range,
        attack_enemies, attack_random, &firing_direction
    );
    if (agent == NULL) return NULL;

    // normalize and bias vector
    normalize_vector(&firing_direction);
    if (accuracy_bias)   // apply bias
        firing_direction = vec3_bias_random(firing_direction, accuracy_bias);

    // get target
    Hitscan::HitscanTarget t = Hitscan::shoot_at_agent(
        position, firing_direction, id, type,
        agent, sight_range
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

Agent_state* TargetAcquisitionComponent::fire_on_known_target(
    int id, Object_types type, float camera_z, Vec3 position, Vec3 direction,
    float accuracy_bias, float sight_range, Agent_state* agent
) {    
    // lock on agent
    position.z = camera_z;

    // normalize and bias vector
    normalize_vector(&direction);
    if (accuracy_bias)   // apply bias
        direction = vec3_bias_random(direction, accuracy_bias);

    // get target
    Hitscan::HitscanTarget t = Hitscan::shoot_at_agent(
        position, direction, id, type,
        agent, sight_range
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
