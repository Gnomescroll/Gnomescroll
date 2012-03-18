#pragma once

#include <c_lib/items/pickup.hpp>

namespace ItemDrops
{

/* Grenade refill */

const int MAX_GRENADE_REFILLS = 256;

class GrenadeRefill: public PickupItem
{
    public:
        void init();
        GrenadeRefill(int id, float x, float y, float z, float mx, float my, float mz);
};

class GrenadeRefill_list: public PickupItem_list<GrenadeRefill, MAX_GRENADE_REFILLS>
{
    private:
        const char* name() { return "GrenadeRefill"; }
    public:
        GrenadeRefill_list() { print(); }
};


/* Laser refill */

const int MAX_LASER_REFILLS = 256;

class LaserRefill: public PickupItem
{
    public:
        void init();
        LaserRefill(int id, float x, float y, float z, float mx, float my, float mz);
};

class LaserRefill_list: public PickupItem_list<LaserRefill, MAX_LASER_REFILLS>
{
    private:
        const char* name() { return "LaserRefill"; }
    public:
        LaserRefill_list() { print(); }
};

} // ItemDrops
