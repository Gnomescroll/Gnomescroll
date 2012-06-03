#pragma once

#include <common/template/simple_object_list.hpp>
#include <physics/verlet_particle.hpp>

namespace Particle
{

void init_shrapnel();
void teardown_shrapnel();

class Shrapnel
{
    public:

        int id;
        int ttl;

        VerletParticle::VerletParticle verlet;

        float scale;
        int texture_index;

        inline void init();
        void prep();
        void tick();

        void init(float x, float y, float z, float mx, float my, float mz);
};

const int SHRAPNEL_MAX = 4096;
class Shrapnel_list: public Simple_object_list<Shrapnel, SHRAPNEL_MAX>
{
    private:
        const char* name() { return "Shrapnel"; }
    public:

        void prep();
        void draw();
        void tick();

        Shrapnel_list() { print_list((char*)this->name(), this); }
};
    
}
