#pragma once

namespace Particle
{


const int GRENADE_SHRAPNEL_MAX = 4096;
const int GRENADE_SHRAPNEL_TTL = 20;
const float GRENADE_SHRAPNEL_DAMP = 1.0f;
const int GRENADE_SHRAPNEL_TYPE = 9;

class Grenade_shrapnel: public ParticleMotion
{
    public:
        void init();
        void tick();

        int owner;

        Grenade_shrapnel();
};

}

#include <common/template/object_list.hpp>

namespace Particle
{


class Grenade_shrapnel_list: public Simple_object_list<Grenade_shrapnel, GRENADE_SHRAPNEL_MAX>
{
    private:
        const char* name() { return "Grenade_shrapnel"; }
    public:
        void tick();

        Grenade_shrapnel_list() { print_list((char*)this->name(), this); }
};
    
}
