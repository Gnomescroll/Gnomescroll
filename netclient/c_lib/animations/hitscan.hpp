#pragma once

#include <c_lib/template/object_list.hpp>

namespace Animations
{

class HitscanEffect
{
    public:
    int id;
    float x,y,z;
    float vx,vy,vz;
    int ttl;

    inline void draw(float delta, Vector* camera) __attribute((always_inline));
    void tick();

    HitscanEffect(int id);
    HitscanEffect(int x, int y, int z, int vx, int vy, int vz);
    HitscanEffect(int id, int x, int y, int z, int vx, int vy, int vz);
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
