#include "weapon_targeting.hpp"

#include <c_lib/physics/ray_trace/hitscan.hpp>
#include <c_lib/physics/ray_trace/handlers.hpp>
#include <c_lib/physics/vec3.hpp>
#include <c_lib/entity/network/packets.hpp>

namespace Components
{

/* Targeting */

void WeaponTargetingComponent::lock_target(Vec3 camera_position)
{
    this->lock_target(camera_position, NO_TEAM);
}

void WeaponTargetingComponent::lock_target(Vec3 camera_position, int team)
{   // lock on agent
    Agent_state* target;
    target = Hitscan::lock_agent_target(
        camera_position, &this->target_direction, team,
        this->sight_range, this->target_acquisition_probability,
        this->attacks_enemies, this->attack_at_random
    );
    if (target == NULL)
    {
        this->target_type = OBJECT_NONE;
        this->locked_on_target = false;
        return;
    }
    this->target_type = OBJECT_AGENT;
    this->target_id = target->id;
    this->locked_on_target = true;
    normalize_vector(&this->target_direction);
}

bool WeaponTargetingComponent::fire_on_target(Vec3 camera_position)
{    
    return this->fire_on_target(camera_position, NO_TEAM);
}

bool WeaponTargetingComponent::fire_on_target(Vec3 camera_position, int team)
{
    if (this->target_type == OBJECT_NONE) return false;
    if (this->target_type != OBJECT_AGENT) return false;    // todo -- target all types
    
    // get target
    Agent_state* target = STATE::agent_list->get(this->target_id);
    if (target == NULL) return false;

    // aim at target
    this->orient_to_random_target_part(camera_position);
    Vec3 direction = this->target_direction;
    if (this->uses_bias)    // apply bias
        direction = vec3_bias_random(this->target_direction, this->accuracy_bias);

    Hitscan::HitscanTarget t = Hitscan::shoot_at_agent(
        camera_position, direction, this->object->id, this->object->type,
        target, this->sight_range
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

void WeaponTargetingComponent::orient_to_random_target_part(Vec3 camera_position)
{
    if (this->target_type == OBJECT_NONE) return;
    if (this->target_type != OBJECT_AGENT) return;  //  todo -- target all types
    Agent_state* target = STATE::agent_list->get(this->target_id);
    if (target == NULL || target->vox == NULL) return;
    int part = randrange(0, target->vox->n_parts-1);
    Vec3 target_position = target->vox->get_center(part);
    this->target_direction = vec3_sub(target_position, camera_position);
    normalize_vector(&this->target_direction);
}

void WeaponTargetingComponent::broadcast_target_choice()
{
    object_choose_target_StoC msg;
    msg.id = this->object->id;
    msg.type = this->object->type;
    msg.target_id = this->target_id;
    msg.target_type = this->target_type;
    msg.broadcast();
}

} // Objects
