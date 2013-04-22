#pragma once

#if DC_SERVER
dont_include_this_file_in_server
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

    void draw_quad(Vec3 p, float r, float theta, float phi);
    inline void draw(Vec3 camera);
    inline void tick();

    void set_state(struct Vec3 start, struct Vec3 end)
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
    void draw();
    void tick();
    RailTrailEffectList()
    {
    }
};

}   // Animations
