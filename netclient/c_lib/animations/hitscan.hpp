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

    void draw();
    void tick();

    //HitscanEffect()
    //:
    //ttl(3000)
    //{}
    
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

}
