#pragma once

#include <c_lib/template/object_list.hpp>

namespace Animations
{

class HitscanLaserEffect
{
    public:
    int id;
    float x,y,z;
    float fx,fy,fz;
    int ttl;

    inline void draw(float delta, Vector* camera) __attribute((always_inline));
    void tick();

    HitscanLaserEffect(int id);
    HitscanLaserEffect(float x, float y, float z, float fx, float fy, float fz);
    HitscanLaserEffect(int id, float x, float y, float z, float fx, float fy, float fz);
};

class HitscanLaserEffect_list: public Object_list<HitscanLaserEffect, MINIVOX_MAX>
{
    private:
        const char* name() { return "HitscanLaserEffect"; }
    public:
        void draw();
        void tick();

        HitscanLaserEffect_list() {}
};

}
