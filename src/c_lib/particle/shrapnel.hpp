#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/template/simple_object_list.hpp>
#include <physics/verlet.hpp>

namespace Particle
{

void init_shrapnel();
void teardown_shrapnel();

class Shrapnel
{
    public:

        int id;
        int ttl;

        Verlet::VerletComponent verlet;

        float scale;
        int texture_index;

        inline void reset();
        void prep();
        void tick();

        void set_state(float x, float y, float z, float mx, float my, float mz)
        {
            this->verlet.position = vec3_init(x,y,z);
            this->verlet.velocity = vec3_init(mx,my,mz);
        }

        Shrapnel() { this->reset(); }
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

        Shrapnel_list() { this->print(); }
};
    
}   // Particle
