#include "destination_targeting.hpp"

#include <physics/vec3.hpp>
#include <physics/motion.hpp>
#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/handlers.hpp>
#include <entity/network/packets.hpp>
#include <common/random.hpp>

namespace Components
{

void DestinationTargetingComponent::set_destination(Vec3 dest)
{
    this->destination = translate_position(dest);
    this->at_destination = false;
    this->target_type = OBJECT_DESTINATION;
}

void DestinationTargetingComponent::choose_destination()
{
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return;
    struct Vec3 position = physics->get_position();

    if (this->path_finished())
    {
        this->ipath = 0;
        this->mpath = 0;
        free(this->path);
        this->path = NULL;
        struct Vec3i current = vec3i_init(position);
        const float r2 = this->stop_proximity * this->stop_proximity;
        const int max_tries = 4;
        for (int i=0; i<max_tries && this->path == NULL; i++)
        {
            struct Vec3i delta = vec3i_init(0);
            do
            {
                delta.x = randrange(this->destination_choice_x/2, this->destination_choice_x);
                delta.y = randrange(this->destination_choice_y/2, this->destination_choice_y);
            } while ((delta.x*delta.x + delta.y*delta.y) < r2);
            struct Vec3i end = vec3i_add(current, delta);
            delta.z = t_map::get_nearest_surface_block(delta.x, delta.y, current.z);
            this->path = Path::get_path_3d_jump(current, end, this->mpath);
        }
        int height = 1;
        DimensionComponent* dims = (DimensionComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
        if (dims != NULL)
            height = dims->get_integer_height();
    }

    if (!this->path_finished())
        position = vec3_init(this->path[this->ipath++]);

    this->set_destination(position);
    this->ticks_to_destination = this->get_ticks_to_destination(physics->get_position());
}

void DestinationTargetingComponent::orient_to_target(Vec3 camera_position)
{
    GS_ASSERT(is_boxed_position(camera_position));
    GS_ASSERT(is_boxed_position(this->destination));
    Vec3 target_position = quadrant_translate_position(camera_position, this->destination);
    this->target_direction = vec3_sub(target_position, camera_position);
    this->target_direction.z = 0.0f;
    if (vec3_length_squared(this->target_direction) == 0.0f)
    {
        this->at_destination = true;
        this->target_direction = vec3_init(1,0,0);
        return;
    }
    normalize_vector(&this->target_direction);
}

// adjusts position & momentum by moving over the terrain surface
bool DestinationTargetingComponent::move_on_surface()
{
    // get physics data
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return false;

    IF_ASSERT(this->speed == 0.0f)
    {
        physics->set_momentum(vec3_init(0,0,0));
        return false;
    }

    // adjust position/momentum by moving along terrain surface
    Vec3 new_position;
    Vec3 new_momentum;

    Vec3 motion_direction = this->target_direction;
    motion_direction.z = 0.0f;

    IF_ASSERT(vec3_length_squared(motion_direction) == 0.0f)
    {
        physics->set_momentum(vec3_init(0,0,0));
        return false;
    }
    normalize_vector(&motion_direction);

    bool moved = move_within_terrain_surface(physics->get_position(), motion_direction,
                                            this->speed, this->max_z_diff,
                                            &new_position, &new_momentum);
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
    IF_ASSERT(physics == NULL)
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

} // Entities
