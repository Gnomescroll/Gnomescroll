#include "component.hpp"

/* TargetAcquisition */

Agent_state* TargetAcquisitionComponent::acquire_target(
    int id, Object_types type, int team, float camera_z, Vec3 position,
    float accuracy_bias, float sight_range,
    bool attack_enemies, bool attack_random
) {    
    // lock on agent
    Vec3 firing_position = vec3_init(position.x, position.y, camera_z);
    Vec3 firing_direction;
    Agent_state* agent = Hitscan::lock_agent_target(
        firing_position, &firing_direction, team,
        sight_range, this->target_acquisition_probability,
        attack_enemies, attack_random
    );
    if (agent == NULL) return NULL;

    // normalize and bias vector
    normalize_vector(&firing_direction);
    if (accuracy_bias)   // apply bias
        firing_direction = vec3_bias_random(firing_direction, accuracy_bias);

    // get target
    Hitscan::HitscanTarget t = Hitscan::shoot_at_agent(
        firing_position, firing_direction, id, type,
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
