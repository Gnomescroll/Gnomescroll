#pragma once

#include <c_lib/common/template/object_list.hpp>

/*
 *
 * NOT USED. go to hitscan.hpp
 *
 */

namespace Animations
{

const int hitscan_lader_ttl = 30;

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

    explicit HitscanLaserEffect(int id);
    HitscanLaserEffect(float x, float y, float z, float fx, float fy, float fz);
    HitscanLaserEffect(int id, float x, float y, float z, float fx, float fy, float fz);
};

class HitscanLaserEffect_list: public Object_list<HitscanLaserEffect>
{
    private:
        const char* name() { return "HitscanLaserEffect"; }
    public:
        void draw();
        void tick();

        HitscanLaserEffect_list() { print_list((char*)this->name(), this); }
};

}
