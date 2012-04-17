#pragma once

#include <c_lib/particles/billboard_sprite.hpp>

#include <c_lib/template/simple_object_list.hpp>

namespace Particles
{

void init_shrapnel();
void teardown_shrapnel();

#define SHRAPNEL_MAX 4096
#define SHRAPNEL_TTL 30
#define SHRAPNEL_DAMP 0.5f
#define SHRAPNEL_TYPE 5
#define SHRAPNEL_TEXTURE_ID 5
#define SHRAPNEL_TEXTURE_SCALE 0.15f

//class Shrapnel: public ParticleMotion, public BillboardSprite

class Shrapnel
{
    public:
        void init();
        void tick();

        void init(float x, float y, float z, float mx, float my, float mz);
};


class Shrapnel_list: public Simple_object_list<Shrapnel, SHRAPNEL_MAX>
{
    private:
        const char* name() { return "Shrapnel"; }
    public:

        int id;

        VerletComponent verlet;
        int id;
        int ttl;
        //int ttl_max;

        void prep();
        void draw();
        void tick();

        Shrapnel_list() { print(); }
};
    
}
