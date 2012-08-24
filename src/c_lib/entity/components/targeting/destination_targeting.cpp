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

void DestinationTargetingComponent::set_destination(Vec3 dest)
{
    this->destination = translate_position(dest);
    this->at_destination = false;
    this->target_type = OBJECT_DESTINATION;
}

void DestinationTargetingComponent::choose_destination()
{
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return;

    float x,y;
    const float r2 = this->stop_proximity * this->stop_proximity;
    do
    {
        x = 2*(randf()-0.5f) * this->destination_choice_x;
        y = 2*(randf()-0.5f) * this->destination_choice_y;
    } while ((x*x + y*y) < r2);

    Vec3 position = physics->get_position();
    position.x += x;
    position.y += y;
    position.z = t_map::get_highest_open_block(position.x, position.y, position.z);
        
    this->set_destination(position);

    this->ticks_to_destination = this->get_ticks_to_destination(physics->get_position());
}

void DestinationTargetingComponent::orient_to_target(Vec3 camera_position)
{
    ASSERT_BOXED_POSITION(camera_position);
    ASSERT_BOXED_POSITION(this->destination);
    Vec3 target_position = quadrant_translate_position(camera_position, this->destination);
    this->target_direction = vec3_sub(target_position, camera_position);
    this->target_direction.z = 0.0f;
    if (vec3_length_squared(this->target_direction) == 0.0f)
    {
        //GS_ASSERT(false);
        this->at_destination = true;
        this->target_direction = vec3_init(1,0,0);
        return;
    }
    normalize_vector(&this->target_direction);
    //printf("target direction: "); vec3_print(this->target_direction);
}

// adjusts position & momentum by moving over the terrain surface
bool DestinationTargetingComponent::move_on_surface()
{
    // get physics data
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return false;

    GS_ASSERT(this->speed != 0.0f);
    if (this->speed == 0.0f)
    {
        physics->set_momentum(vec3_init(0,0,0));
        return false;
    }

    // adjust position/momentum by moving along terrain surface
    Vec3 new_position;
    Vec3 new_momentum;

    Vec3 motion_direction = this->target_direction;
    motion_direction.z = 0.0f;

    if (vec3_length_squared(motion_direction) == 0.0f)
    {
        GS_ASSERT(false);
        physics->set_momentum(vec3_init(0,0,0));
        return false;
    }
    normalize_vector(&motion_direction);

    bool moved = move_along_terrain_surface(
        physics->get_position(), motion_direction,
        this->speed, this->max_z_diff,
        &new_position, &new_momentum
    );
    physics->set_position(new_position);
    physics->set_momentum(new_momentum);

    if (moved && this->ticks_to_destination > 0)
        this->ticks_to_destination--;

    return moved;
}

bool DestinationTargetingComponent::check_at_destination()
{
    if (this->at_destination) return true;
    
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
    pos.z = 0;
    dest.z = 0;
    this->at_destination = (vec3_distance_squared(pos, dest) <= this->stop_proximity*this->stop_proximity);
    return this->at_destination;
}

} // Objects
