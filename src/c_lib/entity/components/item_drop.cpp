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

static struct Vec3 get_dropped_item_position(Objects::Object* object)
{
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
        if (physics == NULL) return vec3_init(0.0f, 0.0f, 0.0f);
        position = physics->get_position();

        // try to center it by using height dimension, if available
        DimensionComponent* dims = (DimensionComponent*)object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
        if (dims != NULL) position.z += dims->get_height() / 2.0f;
    }

    position.x = position.x + (randf()*2*radius) - radius;
    position.y = position.y + (randf()*2*radius) - radius;

	return position;
}

static void create_dropped_item(int item_type, int amount, Vec3 position)
{
	GS_ASSERT(amount > 0);

	float x = position.x;
	float y = position.y;
	float z = position.z;
	
    const float mom = 2.0f;
    float vx,vy,vz = mom;

	for (int i=0; i<amount; i++)
	{
		// create item
		Item::Item* item = Item::create_item(item_type);
		GS_ASSERT(item != NULL);
		if (item == NULL) break;
		
		// create item particle
		vx = (randf()-0.5f)*mom;
		vy = (randf()-0.5f)*mom;
		ItemParticle::ItemParticle* item_particle = ItemParticle::create_item_particle(
			item->id, item->type, x, y, z, vx, vy, vz);
		GS_ASSERT(item_particle != NULL);
		if (item_particle == NULL)
		{
			Item::destroy_item(item->id);
			break;
		}

		// broadcast
		ItemParticle::broadcast_particle_item_create(item_particle->id);
	}
}

void ItemDropEntry::drop_item(Vec3 position)
{
	GS_ASSERT(this->n_drops > 0);
	GS_ASSERT(this->item_type != NULL_ITEM_TYPE);
	GS_ASSERT(this->amount != NULL);
	GS_ASSERT(this->probability != NULL);
	if (this->item_type == NULL_ITEM_TYPE) return;
	if (this->amount == NULL) return;
	if (this->probability == NULL) return;

	float p = randf();
	float p_start = 0.0f;
	for (int i=0; i<this->n_drops; i++)
	{
		float drop_p = this->probability[i];
		if (p >= p_start && p <= p_start+drop_p)
		{	// drop
			create_dropped_item(this->item_type, this->amount[i], position);
			break;
		}
		p_start += drop_p;
		GS_ASSERT(p_start < 1.0001f);
	}
}

void ItemDropComponent::drop_item()
{
	// treat each item type independently
	// but within each item type treat cumulatively and exclusively
	
	GS_ASSERT(this->drop != NULL);
	if (this->drop == NULL) return;
	GS_ASSERT(this->max_drops > 0);
	GS_ASSERT(this->n_drops == max_drops);
	
	Vec3 position = get_dropped_item_position(this->object);
	
	for (int i=0; i<this->max_drops; i++)
		this->drop[i].drop_item(position);
}

}   // Components
