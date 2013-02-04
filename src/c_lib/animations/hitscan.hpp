#pragma once

#if DC_SERVER
dont_include_this_file_in_server
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

    inline void draw(float delta, Vec3 camera) __attribute((always_inline));
    inline void tick();

    void set_state(struct Vec3 p, struct Vec3 v)
    {
        ASSERT_BOXED_POSITION(p);
        struct Vec3 scaled = vec3_scalar_mult(vec3_abs(v), 1.0f/30.0f);
        GS_ASSERT(scaled.x < 128.0f);
        GS_ASSERT(scaled.y < 128.0f);
        GS_ASSERT(scaled.z < 128.0f);
        this->p = p;
        this->v = v;
    }

    void reset();

    HitscanEffect()
    {
        this->reset();
    }
};

class HitscanEffectList: public Simple_object_list<HitscanEffect, 1024>
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
