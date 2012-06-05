#pragma once

#include <common/template/object_list.hpp>

namespace Animations
{

void init_hitscan();
void teardown_hitscan();

class HitscanEffect
{
    private:
    // DO NOT USE:: TESTING PURPOSES ONLY
    void add_plane_bias();   // for player agent, so laser isnt in the eyes
    
    public:
    int id;
    float x,y,z;
    float vx,vy,vz;
    int ttl;

    inline void draw(float delta, Vec3 camera) __attribute((always_inline));
    inline void tick();

    void set_state(float x, float y, float z, float vx, float vy, float vz)
    {
        this->x = x; this->y = y; this->z = z;
        this->vx = vx; this->vy = vy; this->vz = vz;
    }

    void init();
};

class HitscanEffect_list: public Simple_object_list<HitscanEffect>
{
    private:
        const char* name() { return "HitscanEffect"; }
    public:
        void draw();
        void tick();

        HitscanEffect_list() { print_list((char*)this->name(), this); }
};

}   // Animations
