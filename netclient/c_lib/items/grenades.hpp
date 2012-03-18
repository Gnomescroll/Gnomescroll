#pragma once

#include <c_lib/particles/textures.hpp>
#include <c_lib/particles/particles.hpp>
#include <c_lib/common/enum_types.hpp>

namespace ItemDrops
{

const float ITEM_PICKUP_RADIUS = 1.5f;

class PickupItem
{
    private:
        const float radius;
    public:
        int nearest_agent_in_range(Vec3 p);
        PickupItem(const float radius) : radius(ITEM_PICKUP_RADIUS) {}
};

const int GRENADES_TEXTURE_ID = 240;
const float GRENADES_TEXTURE_SCALE = 1.0f;
const float GRENADES_MASS = 1.0f;
const int GRENADES_TTL = 30 * 12;   // 12 seconds
const float GRENADES_DAMP = 0.1f;

class Grenades: public Particle, public BillboardSprite, public PickupItem
{
    public:
        bool broadcast_death;
        
        void init();
        void tick();
        void schedule_death_broadcast();

        void born();
        void die();
        
        Grenades(int id);
        Grenades(int id, float x, float y, float z, float mx, float my, float mz);
};

const int MAX_GRENADES = 256;
class GrenadeDrops_list: public Object_list<Grenades, MAX_GRENADES>
{
    private:
        const char* name() { return "Grenade drops"; }
    public:
        void tick();
        void draw();

        GrenadeDrops_list() { print(); }
};


} // ItemDrops


// 
// track in an object list
