#include "item_drop.hpp"

#if DC_CLIENT
# error Don't include this file in the client
#endif

#include <item/_interface.hpp>
#include <item/item.hpp>
#include <item/particle/_interface.hpp>
#include <item/particle/item_particle.hpp>

namespace Components
{

static struct Vec3 get_dropped_item_position(Entities::Entity* object)
{
    // get object state
    Vec3 position;
    float radius = 0.95f;

    // try to use center of voxel model
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    if (vox != NULL)
    {
        position = vox->get_center();
        radius = vox->get_radius();
    }
    else
    {   // use physics position
        PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        GS_ASSERT(physics != NULL);
        if (physics == NULL) return vec3_init(0.0f, 0.0f, 0.0f);
        position = physics->get_position();

        // try to center it by using height dimension, if available
        DimensionComponent* dims = (DimensionComponent*)object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
        if (dims != NULL) position.z += dims->get_height() / 2.0f;
    }

    // assume item is centered
    position.x = position.x + radius*(randf() - 0.5f);
    position.y = position.y + radius*(randf() - 0.5f);

    position = translate_position(position);

    return position;
}

void ItemDropComponent::drop_item()
{
    Vec3 position = get_dropped_item_position(this->object);
    this->drop.drop_item(position);
}


}   // Components
