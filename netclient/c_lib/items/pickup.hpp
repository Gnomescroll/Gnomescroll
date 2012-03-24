#pragma once

#include <c_lib/particles/textures.hpp>
#include <c_lib/particles/particles.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/template/object_list.hpp>

#include <c_lib/physics/verlet.hpp>
#include <c_lib/items/packets.hpp>

namespace ItemDrops
{

const int DEFAULT_PICKUP_ITEM_TTL = 30 * 12;    // 12 seconds
const float DEFAULT_PICKUP_ITEM_RADIUS = 2.0f;
const float DEFAULT_PICKUP_ITEM_TEXTURE_SCALE = 1.0f;
const float DEFAULT_PICKUP_ITEM_DAMP = 0.1f;    // hardly bounce
const float DEFAULT_PICKUP_ITEM_MASS = 1.0f;

/*
 *  Don't instantiate any of these classes directly
 *
 * See c_lib/items/grenades.*pp for an example of how to use
 *
 */

class Pickup
{
    private:
        const float radius;
    public:
        int nearest_agent_in_range(Vec3 p);
        explicit Pickup(const float radius) : radius(radius) {}
};

class PickupItem: public Particle, public BillboardSprite, public Pickup
{
    private:
        bool broadcast_death;
        int picked_up_by;
    public:
        virtual void init();
        void tick();
        void was_picked_up(int agent_id);

        void born();
        void die();
        
        explicit PickupItem(int id);
        PickupItem(int id, float x, float y, float z, float mx, float my, float mz);
};

template<class Obj, int obj_max>
class PickupItem_list: public Object_list<Obj, obj_max>
{
    public:
        void tick();
        void draw();
        PickupItem_list() {}
};

} // ItemDrops
