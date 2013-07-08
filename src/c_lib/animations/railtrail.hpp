#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <common/template/object_list.hpp>

namespace Animations
{

class RailTrailEffect
{
    public:
        int id;
        struct Vec3 start;
        struct Vec3 end;
        int ttl;

    void draw_quad(const Vec3& p, float r, float theta, float phi);
    //inline void draw(const Vec3& camera);
    void tick();

    void set_state(Vec3& start, Vec3& end)
    {
        GS_ASSERT(is_boxed_position(start));
        this->start = start;
        this->end = end;
    }

    void reset();

    RailTrailEffect()
    {
        this->reset();
    }
};

class RailTrailEffectList: public SimpleObjectList<RailTrailEffect, 1024>
{
    private:
    const char* name()
    {
        return "RailTrailEffect";
    }
    public:
    //void draw();
    void tick();
    RailTrailEffectList()
    {
    }
};

}   // Animations
