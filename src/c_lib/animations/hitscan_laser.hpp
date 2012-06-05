#pragma once

#include <common/template/object_list.hpp>

/*
 *
 * NOT USED. go to hitscan.hpp
 *
 */

namespace Animations
{

const int HITSCAN_LASER_TTL = 30;

void init_hitscan_laser();
void teardown_hitscan_laser();

class HitscanLaserEffect
{
    public:
    int id;
    float x,y,z;
    float fx,fy,fz;
    int ttl;

    inline void draw1(float delta, Vec3 camera) __attribute((always_inline));
    inline void draw2(float delta, Vec3 camera) __attribute((always_inline));
    void tick();

    inline void init();
};

class HitscanLaserEffect_list: public Simple_object_list<HitscanLaserEffect>
{
    private:
        const char* name() { return "HitscanLaserEffect"; }
    public:
        void draw();
        void tick();

        HitscanLaserEffect_list() { print_list((char*)this->name(), this); }
};

}   // Animations
