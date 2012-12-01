#pragma once

#include <common/template/object_list.hpp>
#include <t_gen/explosives.hpp>

namespace Particle
{

const int PLASMAGEN_SPUR_MAX = 1024;
const float PLASMAGEN_SPUR_DAMP = 1.0f;
const int PLASMAGEN_SPUR_TEXTURE_ID = 54;
const float PLASMAGEN_SPUR_TEXTURE_SCALE = 0.25f;

class PlasmagenSpur: public ParticleMotion, public BillboardSprite
{
    public:
        static const int texture_index = PLASMAGEN_SPUR_TEXTURE_ID;
        float travelled;

    inline void reset();
    void tick();
    void die();

    PlasmagenSpur();
};

class PlasmagenSpurList: public Simple_object_list<PlasmagenSpur, PLASMAGEN_SPUR_MAX>
{
    private:
        const char* name() { return "PlasmagenSpur"; }
    public:
        #if DC_CLIENT
        void draw();
        #endif
        void tick();

    PlasmagenSpurList() {}
};
    
}   // Particle
