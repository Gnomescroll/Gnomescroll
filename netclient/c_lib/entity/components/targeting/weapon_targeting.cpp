#include "weapon_targeting.hpp"

#include <c_lib/ray_trace/hitscan.hpp>
#include <c_lib/ray_trace/handlers.hpp>
#include <c_lib/physics/vec3.hpp>

namespace Components
{

/* Targeting */

Vec3 WeaponTargetingComponent::lock_target(Vec3 camera_position, int team)
{   // lock on agent
    this->target = Hitscan::lock_agent_target(
        camera_position, &this->target_direction, team,
        this->sight_range, this->target_acquisition_probability,
        this->attacks_enemies, this->attack_at_random
    );
    if (this->target != NULL) normalize_vector(&this->target_direction);
    return this->target_direction;
}

Vec3 WeaponTargetingComponent::lock_target(Vec3 camera_position)
{
    return this->lock_target(camera_position, NO_TEAM);
}

bool WeaponTargetingComponent::fire_on_target(Vec3 camera_position)
{    
    return this->fire_on_target(camera_position, NO_TEAM);
}

bool WeaponTargetingComponent::fire_on_target(Vec3 camera_position, int team)
{
    if (this->target == NULL) return false;
    
    Vec3 direction;
    if (this->accuracy_bias)    // apply bias
        direction = vec3_bias_random(this->target_direction, this->accuracy_bias);

    // get target
    Hitscan::HitscanTarget t = Hitscan::shoot_at_agent(
        camera_position, direction, this->object->id, this->object->type,
        this->target, this->sight_range
    );

    // let handle target hit based on attacker properties
    Hitscan::handle_hitscan_target(t, this->attacker_properties);

    // send firing packet
    Hitscan::broadcast_object_fired(this->object->id, this->object->type, t);

    // apply custom handling
    // play sounds
    // play animations
    
    if (t.hitscan == Hitscan::HITSCAN_TARGET_VOXEL) return true;
    return false;
}

} // Objects
