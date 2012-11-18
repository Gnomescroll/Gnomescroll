#include "weapon_targeting.hpp"

#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/handlers.hpp>
#include <physics/vec3.hpp>
#include <entity/network/packets.hpp>
#include <agent/_interface.hpp>

namespace Components
{

/* Targeting */

void WeaponTargetingComponent::lock_target(Vec3 camera_position)
{   // lock on agent
    Agent* target = Hitscan::lock_agent_target(
        camera_position, &this->target_direction,
        this->sight_range, this->target_acquisition_failure_rate,
        this->attack_at_random
    );
    if (target == NULL)
    {
        this->target_type = OBJECT_NONE;
        this->locked_on_target = false;
        return;
    }
    this->target_type = target->type;
    this->target_id = target->id;
    this->locked_on_target = true;
    normalize_vector(&this->target_direction);
}

void WeaponTargetingComponent::lock_target_part(Vec3 camera_position)
{
    this->firing_direction_set = false;

    if (this->target_type == OBJECT_NONE) return;
    if (this->target_type != OBJECT_AGENT) return;    // TODO -- target all types

    // get target
    Agent* target = Agents::get_agent((AgentID)this->target_id);
    if (target == NULL) return;

    // aim at target
    this->orient_to_random_target_part(camera_position);
    Vec3 direction = this->target_direction;
    if (this->uses_bias)    // apply bias
        direction = vec3_bias_random(this->target_direction, this->accuracy_bias);
    
    this->firing_direction = direction;
    this->firing_direction_set = true;
}

bool WeaponTargetingComponent::fire_on_target(Vec3 camera_position)
{    
    if (this->target_type == OBJECT_NONE) return false;
    if (this->target_type != OBJECT_AGENT) return false;    // TODO -- target all types
    
    if (!this->firing_direction_set) return false;
    
    // get target
    Agent* target = Agents::get_agent((AgentID)this->target_id);
    if (target == NULL) return false;
    
    Hitscan::HitscanTarget t = Hitscan::shoot_at_agent(
        camera_position, this->firing_direction, this->object->id, this->object->type,
        target, this->sight_range
    );

    // let handle target hit based on attacker properties
    Hitscan::handle_hitscan_target(t, this->attacker_properties);

    // send firing packet
    Hitscan::broadcast_object_fired(this->object->id, this->object->type, t);

    // apply custom handling
    // play sounds
    // play animations
    
    if (t.hitscan == HITSCAN_TARGET_VOXEL) return true;
    return false;
}

bool WeaponTargetingComponent::target_is_visible(Vec3 firing_position)
{
    Agent* target = Agents::get_agent((AgentID)this->target_id);
    // target exists
    if (target == NULL) return false;
    // target in range
    Vec3 target_position = target->vox->get_center();
    target_position = quadrant_translate_position(firing_position, target_position);
    if (vec3_distance_squared(target_position, firing_position) > this->sight_range*this->sight_range)
        return false;
    // target visible
    Vec3 sink;
    if (target->in_sight_of(firing_position, &sink)) return true;
    return false;
}

void WeaponTargetingComponent::orient_to_random_target_part(Vec3 camera_position)
{
    if (this->target_type == OBJECT_NONE) return;
    if (this->target_type != OBJECT_AGENT) return;  //  todo -- target all types
    Agent* target = Agents::get_agent((AgentID)this->target_id);
    if (target == NULL || target->vox == NULL) return;
    int part = randrange(0, target->vox->n_parts-1);
    Vec3 target_position = target->vox->get_center(part);
    target_position = quadrant_translate_position(camera_position, target_position);
    this->target_direction = vec3_sub(target_position, camera_position);
    normalize_vector(&this->target_direction);
}

void WeaponTargetingComponent::broadcast_target_choice()
{
    GS_ASSERT(this->object != NULL);
    if (this->object == NULL) return;
    object_choose_weapon_target_StoC msg;
    msg.id = this->object->id;
    msg.type = this->object->type;
    msg.target_id = this->target_id;
    msg.target_type = this->target_type;
    msg.broadcast();
}

} // Entities
