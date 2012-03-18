#pragma once

#include <c_lib/particles/textures.hpp>
#include <c_lib/particles/particles.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/template/object_list.hpp>

/*
 *  Don't instantiate any of these classes directly
 *
 * See c_lib/items/grenades.*pp for an example of how to use
 *
 */

namespace ItemDrops
{

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
    public:
        virtual void init();
        void tick();
        void schedule_death_broadcast();

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
