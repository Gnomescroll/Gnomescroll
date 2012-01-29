#pragma once

#include <c_lib/template/object_list.hpp>

namespace Animations
{

class HitscanEffect
{
    private:
    
    public:
    int id;
    float x,y,z;
    float vx,vy,vz;
    int ttl;

    inline void draw(float delta, Vector* camera) __attribute((always_inline));
    void tick();

    void add_plane_bias();   // for player agent, so laser isnt in the eyes

    HitscanEffect(int id);
    HitscanEffect(float x, float y, float z, float vx, float vy, float vz);
    HitscanEffect(int id, float x, float y, float z, float vx, float vy, float vz);
};

class HitscanEffect_list: public Object_list<HitscanEffect, MINIVOX_MAX>
{
    private:
        const char* name() { return "HitscanEffect"; }
    public:
        void draw();
        void tick();

        HitscanEffect_list() {}
};

}
