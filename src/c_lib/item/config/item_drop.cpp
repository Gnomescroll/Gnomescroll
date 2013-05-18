#include "item_drop.hpp"

#if DC_CLIENT
# error Don't include this file in the client
#endif

#include <physics/quadrant.hpp>
#include <item/_interface.hpp>
#include <item/item.hpp>
#include <item/particle/_interface.hpp>
#include <item/particle/item_particle.hpp>

namespace Item
{

static void create_dropped_item(ItemType item_type, int amount, Vec3 position, float vx, float vy, float vz, randFloat vx_func, randFloat vy_func, randFloat vz_func)
{
    GS_ASSERT(amount > 0);

    position = translate_position(position);
    float x = position.x;
    float y = position.y;
    float z = position.z;

    for (int i=0; i<amount; i++)
    {
        // create item
        class Item* item = create_item(item_type);
        GS_ASSERT(item != NULL);
        if (item == NULL) break;

        // create item particle
        if (vx_func != NULL) vx = vx_func();
        if (vy_func != NULL) vy = vy_func();
        if (vz_func != NULL) vz = vz_func();
        ItemParticle::ItemParticle* item_particle = ItemParticle::create_item_particle(
            item->id, item->type, x, y, z, vx, vy, vz);
        GS_ASSERT(item_particle != NULL);
        if (item_particle == NULL)
        {
            destroy_item(item->id);
            break;
        }

        // broadcast
        ItemParticle::broadcast_particle_item_create(item_particle->id);
    }
}


void ItemDropEntry::drop_item(Vec3 position, float vx, float vy, float vz, randFloat vx_func, randFloat vy_func, randFloat vz_func)
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
        {    // drop
            create_dropped_item(this->item_type, this->amount[i], position, vx, vy, vz, vx_func, vy_func, vz_func);
            break;
        }
        p_start += drop_p;
        GS_ASSERT(p_start < 1.0001f);
    }
}

void ItemDrop::drop_item(Vec3 position)
{
    // treat each item type independently
    // but within each item type treat cumulatively and exclusively
    GS_ASSERT(this->drop != NULL);
    if (this->drop == NULL) return;
    GS_ASSERT(this->max_drops > 0);
    GS_ASSERT(this->n_drops == max_drops);

    for (int i=0; i<this->max_drops; i++)
        this->drop[i].drop_item(position, vx, vy, vz, vx_func, vy_func, vz_func);
}

}   // Item
