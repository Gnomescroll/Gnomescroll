/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "item_drop.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <physics/quadrant.hpp>
#include <item/_interface.hpp>
#include <item/item.hpp>
#include <item/particle/_interface.hpp>
#include <item/particle/item_particle.hpp>

namespace Item
{

static void create_dropped_item(ItemType item_type, int amount, const Vec3& position,
                                const Vec3& velocity, randFloat vx_func,
                                randFloat vy_func, randFloat vz_func)
{
    GS_ASSERT(amount > 0);
    Vec3 p = translate_position(position);
    for (int i=0; i<amount; i++)
    {   // create item
        class Item* item = create_item(item_type);
        IF_ASSERT(item == NULL) break;
        // create item particle
        Vec3 v = velocity;
        if (vx_func != NULL) v.x = vx_func();
        if (vy_func != NULL) v.y = vy_func();
        if (vz_func != NULL) v.z = vz_func();
        ItemParticle::ItemParticle* item_particle = ItemParticle::create_item_particle(
            item->id, item->type, p, v);
        IF_ASSERT(item_particle == NULL)
        {
            destroy_item(item->id);
            break;
        }
        // broadcast
        ItemParticle::broadcast_particle_item_create(item_particle->id);
    }
}


void ItemDropEntry::drop_item(Vec3 position, const Vec3& velocity,
                              randFloat vx_func, randFloat vy_func,
                              randFloat vz_func)
{
    GS_ASSERT(this->n_drops > 0);
    IF_ASSERT(this->item_type == NULL_ITEM_TYPE) return;
    IF_ASSERT(this->amount == NULL) return;
    IF_ASSERT(this->probability == NULL) return;

    float p = randf();
    float p_start = 0.0f;
    for (int i=0; i<this->n_drops; i++)
    {
        float drop_p = this->probability[i];
        if (p >= p_start && p <= p_start + drop_p)
        {    // drop
            create_dropped_item(this->item_type, this->amount[i], position,
                                velocity, vx_func, vy_func, vz_func);
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
    IF_ASSERT(this->drop == NULL) return;
    GS_ASSERT(this->max_drops > 0);
    GS_ASSERT(this->n_drops == max_drops);

    for (int i=0; i<this->max_drops; i++)
        this->drop[i].drop_item(position, this->velocity, vx_func, vy_func, vz_func);
}

}   // Item
