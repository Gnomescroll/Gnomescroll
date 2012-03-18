#include "grenades.hpp"

#include <c_lib/physics/verlet.hpp>
#include <c_lib/items/packets.hpp>

namespace ItemDrops
{

const int DEFAULT_PICKUP_ITEM_TTL = 30 * 12;    // 12 seconds
const float DEFAULT_PICKUP_ITEM_RADIUS = 1.5f;
const float DEFAULT_PICKUP_ITEM_TEXTURE_SCALE = 1.0f;
const float DEFAULT_PICKUP_ITEM_DAMP = 0.1f;    // hardly bounce
const float DEFAULT_PICKUP_ITEM_MASS = 1.0f;

int Pickup::nearest_agent_in_range(Vec3 p)
{
    int n = STATE::agent_list->objects_within_sphere(p.x, p.y, p.z, radius);
    if (n > 0)
        return STATE::agent_list->filtered_objects[0]->id;
    return -1;
}

/* PickupItem
 *
 * Merges Particle, BillboardSprite and Pickup into one object
 *
 * */

void PickupItem::tick()
{
    Verlet::bounce(this->vp, this->damp);
    #if DC_SERVER
    int agent_id = this->nearest_agent_in_range(this->vp->p);
    if (agent_id >= 0 && STATE::agent_list->agent_pickup_item(agent_id, (Object_types)this->type))
    {   // was picked up, die
        this->schedule_death_broadcast();
        this->ttl = this->ttl_max;
    }
    #endif
    this->ttl++;
}

void PickupItem::schedule_death_broadcast()
{
    this->broadcast_death = true;
}

void PickupItem::die()
{
    #if DC_SERVER
    if (this->broadcast_death)
    {
        item_destroy_StoC msg;
        msg.type = this->type;
        msg.id = this->id;
        msg.broadcast();
    }
    #endif
}

void PickupItem::born()
{
    #if DC_SERVER
    item_create_StoC msg;
    msg.type = this->type;
    msg.id = this->id;
    msg.x = this->vp->p.x;
    msg.y = this->vp->p.y;
    msg.z = this->vp->p.z;
    Vec3 m = this->vp->get_momentum();
    msg.mx = m.x;
    msg.my = m.y;
    msg.mz = m.z;
    msg.broadcast();
    #endif
}

void PickupItem::init()
{
    this->ttl = DEFAULT_PICKUP_ITEM_TTL;
    this->scale = DEFAULT_PICKUP_ITEM_TEXTURE_SCALE;
    this->damp = DEFAULT_PICKUP_ITEM_DAMP;
}

PickupItem::PickupItem(int id)
:
Particle(id, 0,0,0,0,0,0, DEFAULT_PICKUP_ITEM_MASS),
BillboardSprite(),
Pickup(DEFAULT_PICKUP_ITEM_RADIUS),
broadcast_death(false)
{}

PickupItem::PickupItem(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z,mx,my,mz, DEFAULT_PICKUP_ITEM_MASS),
BillboardSprite(),
Pickup(DEFAULT_PICKUP_ITEM_RADIUS),
broadcast_death(false)
{}

/* list */

template<class Obj, int obj_max>
void PickupItem_list<Obj, obj_max>::tick()
{
    if (this->num <= 0) return;
    for (int i=0; i<obj_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->a[i]->tick();
        if (this->a[i]->ttl >= this->a[i]->ttl_max)
        {
            this->a[i]->die();
            this->destroy(this->a[i]->id);
        }
    }
}

template<class Obj, int obj_max>
void PickupItem_list<Obj, obj_max>::draw()
{
    if (this->num <= 0) return;
    for (int i=0; i<obj_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->draw_from_bottom(this->a[i]->vp->p);
}

}   // ItemDrops
