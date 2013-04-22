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

    void set_state(struct Vec3 p, struct Vec3 v)
    {
        this->verlet.position = p;
        this->verlet.velocity = v;
    }

    Shrapnel()
    {
        this->reset();
    }
};

const int SHRAPNEL_MAX = 8192;
class Shrapnel_list: public SimpleObjectList<Shrapnel, SHRAPNEL_MAX>
{
    private:
    const char* name()
    {
        return "Shrapnel";
    }
    public:
    void prep();
    void draw();
    void tick();
    Shrapnel_list() {}
};

}   // Particle
