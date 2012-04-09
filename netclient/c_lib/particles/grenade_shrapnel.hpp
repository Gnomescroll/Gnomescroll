#pragma once

namespace Particles
{


#define GRENADE_SHRAPNEL_MAX 4096
#define GRENADE_SHRAPNEL_TTL 20
#define GRENADE_SHRAPNEL_DAMP 1.0f
#define GRENADE_SHRAPNEL_TYPE 9

class Grenade_shrapnel: public Particle
{
    public:
        void init();
        void tick();

        int owner;

        explicit Grenade_shrapnel(int id);
        Grenade_shrapnel(int id, float x, float y, float z, float mx, float my, float mz);
};

}

#include <c_lib/template/object_list.hpp>

namespace Particles
{


class Grenade_shrapnel_list: public Object_list<Grenade_shrapnel, GRENADE_SHRAPNEL_MAX>
{
    private:
        const char* name() { return "Grenade_shrapnel"; }
    public:
        void tick();

        Grenade_shrapnel_list() { print(); }
};
    
}