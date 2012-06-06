#include "item_drop.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/_interface.hpp>
#include <item/item.hpp>
#include <item/particle/_interface.hpp>
#include <item/particle/item_particle.hpp>

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
    if (randf() > this->probability) return;

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
    float x = position.x + (randf()*2*radius) - radius;
    float y = position.y + (randf()*2*radius) - radius;
    float z = position.z;
    float vx = (randf()-0.5f)*mom;
    float vy = (randf()-0.5f)*mom;
    float vz = mom;

    // create item
    Item::Item* item = Item::create_item(this->item_type);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;
    
    // set stack size
    int max_stack = Item::get_max_stack_size(this->item_type);
    GS_ASSERT(amt <= max_stack);
    if (amt > max_stack) amt = max_stack;
    item->stack_size = amt;

    // create item particle
    ItemParticle::ItemParticle* item_particle = ItemParticle::create_item_particle(
        item->id, item->type, x, y, z, vx, vy, vz);
    GS_ASSERT(item_particle != NULL);
    if (item_particle == NULL) return;

    // broadcast
    ItemParticle::broadcast_particle_item_create(item_particle->id);
}

}   // Components
