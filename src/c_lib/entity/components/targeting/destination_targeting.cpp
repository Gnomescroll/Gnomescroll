#include "destination_targeting.hpp"

#include <physics/vec3.hpp>
#include <physics/motion.hpp>
#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/handlers.hpp>
#include <entity/network/packets.hpp>
#include <common/random.h>

namespace Components
{

/* Targeting */

void DestinationTargetingComponent::choose_destination()
{
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return;

    float x = (randf()-0.5f)*2 * this->destination_choice_x;
    float y = (randf()-0.5f)*2 * this->destination_choice_y;
    float r = this->stop_proximity * this->stop_proximity;
    if (x == 0.0f && y == 0.0f) x = this->destination_choice_x;
    float len = sqrtf(x*x + y*y);
    if (len < r)
    {
        x = this->stop_proximity;
        y = this->stop_proximity;
        len = sqrtf(2.0f) * this->stop_proximity;
    }
    
    this->ticks_to_destination = (int)ceil(len / this->speed);

    Vec3 position = physics->get_position();
    position.x += x;
    position.y += y;
    position.z = t_map::get_highest_solid_block(position.x, position.y);
        
    this->destination = translate_position(position);
    this->at_destination = false;
    this->target_type = OBJECT_DESTINATION;
}

void DestinationTargetingComponent::orient_to_target(Vec3 camera_position)
{
    Vec3 target_position = quadrant_translate_position(camera_position, this->destination);
    this->target_direction = vec3_sub(target_position, camera_position);
    normalize_vector(&this->target_direction);
}

// adjusts position & momentum by moving over the terrain surface
bool DestinationTargetingComponent::move_on_surface()
{
    // get physics data
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return false;

    // adjust position/momentum by moving along terrain surface
    Vec3 new_position;
    Vec3 new_momentum;

    Vec3 motion_direction = this->target_direction;
    motion_direction.z = 0.0f;

    GS_ASSERT(vec3_length(this->target_direction) != 0.0f);

    bool moved = move_along_terrain_surface(
        physics->get_position(), motion_direction,
        this->speed, this->max_z_diff,
        &new_position, &new_momentum
    );
    physics->set_position(new_position);
    physics->set_momentum(new_momentum);

    if (vec3_length(new_momentum))
    {   // update target direction
        new_momentum.z = 0;
        normalize_vector(&new_momentum);
        this->target_direction = new_momentum;
    }
    
    return moved;
}

bool DestinationTargetingComponent::check_at_destination()
{
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL)
    {
        this->at_destination = true;
        return true;
    }
    Vec3 pos = physics->get_position();
    Vec3 dest = quadrant_translate_position(pos, this->destination);
    if (vec3_distance_squared(pos, dest) <= this->stop_proximity*this->stop_proximity)
        this->at_destination = true;
    else
        this->at_destination = false;
        
    return this->at_destination;
}

} // Objects
