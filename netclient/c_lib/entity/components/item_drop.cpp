#include "item_drop.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/_interface.hpp>

namespace Components
{

void ItemDropComponent::drop_item()
{
    GS_ASSERT(this->item_type != NULL_ITEM_TYPE);
    if (this->item_type == NULL_ITEM_TYPE) return;
    GS_ASSERT(this->probability > 0.0f);
    if (this->probability <= 0.0f) return;
    GS_ASSERT(this->max_amount >= 1);
    if (this->max_amount < 1) return;

    // probability check
    if (rand() > this->probability) return;

    // get amount to drop
    int amt = randrange(1, this->max_amount);
    GS_ASSERT(amt >= 1);
    if (amt < 1) return;
    
    // get object state
    Vec3 position;
    float radius = 1.0f;

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
        if (physics == NULL) return;
        position = physics->get_position();

        // try to center it by using height dimension, if available
        DimensionComponent* dims = (DimensionComponent*)object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
        if (dims != NULL) position.z += dims->get_height() / 2.0f;
    }

    const float mom = 2.0f;
    for (int i=0; i<amt; i++)
    {
        // create item particle
        float x = position.x + (randf()*2*radius) - radius;
        float y = position.y + (randf()*2*radius) - radius;
        float z = position.z;
        ItemParticle::create_item_particle(this->item_type, x, y, z, 
            (randf()-0.5f)*mom, (randf()-0.5f)*mom, (randf()-0.5f)*mom); 
    }
}

}   // Components
