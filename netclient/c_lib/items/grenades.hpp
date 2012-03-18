#pragma once

#include <c_lib/items/pickup.hpp>

namespace ItemDrops
{

const int MAX_GRENADES = 256;

class Grenades: public PickupItem
{
    public:
        void init();
        Grenades(int id, float x, float y, float z, float mx, float my, float mz);
};

class GrenadeDrops_list: public PickupItem_list<Grenades, MAX_GRENADES>
{
    private:
        const char* name() { return "Grenade drops"; }
    public:
        GrenadeDrops_list() { print(); }
};

} // ItemDrops
