#pragma once

#include <c_lib/template/object_list.hpp>

#include <c_lib/camera/camera.hpp>

#include <compat_gl.h>
#include <c_lib/SDL/texture_loader.h>
#include <c_lib/physics/vector.hpp>


namespace Animations
{

class HitscanEffect
{
    public:
    int id;
    float x,y,z;
    float vx,vy,vz;
    int ttl;

    void draw(float delta);
    void tick();

    HitscanEffect(int id)
    :
    id(id),
    ttl(3000)
    {}
    
    HitscanEffect(int x, int y, int z, int vx, int vy, int vz)
    :
    x(x), y(y), z(z),
    vx(vx), vy(vy), vz(vz),
    ttl(3000)
    {}
    _x = x;
    _y = y;
    _z = z;
    
    HitscanEffect(int id, int x, int y, int z, int vx, int vy, int vz)
    :
    id(id),
    x(x), y(y), z(z),
    vx(vx), vy(vy), vz(vz),
    ttl(3000)
    {}
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


    int last_tick = _LAST_TICK();
    int _t = _GET_MS_TIME();
    //printf("ms since last update= %i \n", _t - last_tick);
    float delta = ((float)(_t - last_tick)) / 33.3f;
    if(delta > 1.0f)
    {
        delta = 1.0f;
    }
    delta /= 30.0f;
    //printf("delta= %f \n", delta);

}
