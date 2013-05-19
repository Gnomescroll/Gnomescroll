#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

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
        struct Vec3 p;
        struct Vec3 v;
        int ttl;

    inline void draw(Vec3 camera);
    inline void tick();

    void set_state(struct Vec3 p, struct Vec3 v)
    {
        GS_ASSERT(is_boxed_position(p));
        struct Vec3 scaled = vec3_scalar_mult(vec3_abs(v), 1.0f/30.0f);
        GS_ASSERT(scaled.x < map_dim.x);
        GS_ASSERT(scaled.y < map_dim.y);
        GS_ASSERT(scaled.z < map_dim.z);
        this->p = p;
        this->v = v;
    }

    void reset();

    HitscanEffect()
    {
        this->reset();
    }
};

class HitscanEffectList: public SimpleObjectList<HitscanEffect, 1024>
{
    private:
    const char* name()
    {
        return "HitscanEffect";
    }
    public:
    void draw();
    void tick();
    HitscanEffectList()
    {
    }
};

}   // Animations
