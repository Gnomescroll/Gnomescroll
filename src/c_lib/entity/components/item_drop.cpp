#include "item_drop.hpp"

#if DC_CLIENT
# error Do not include this file in the client
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
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    if (vox != NULL)
    {
        position = vox->get_center();
        radius = vox->get_radius();
    }
    else
    {   // use physics position
        auto physics = GET_COMPONENT_INTERFACE(Physics, object);
        GS_ASSERT(physics != NULL);
        if (physics == NULL) return vec3_init(0);
        position = physics->get_position();

        // try to center it by using height dimension, if available
        auto dims = GET_COMPONENT_INTERFACE(Dimension, object);
        if (dims != NULL) position.z += dims->get_height() * 0.5f;
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
