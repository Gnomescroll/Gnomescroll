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
        float x,y,z;
        float vx,vy,vz;
        int ttl;

    inline void draw(float delta, Vec3 camera) __attribute((always_inline));
    inline void tick();

    void set_state(float x, float y, float z, float vx, float vy, float vz)
    {
        ASSERT_BOXED_POINTf(x);
        ASSERT_BOXED_POINTf(y);
        GS_ASSERT(fabsf(vx)/30.0f < 128.0f);
        GS_ASSERT(fabsf(vy)/30.0f < 128.0f);
        GS_ASSERT(fabsf(vz)/30.0f < 128.0f);
        this->x = x; this->y = y; this->z = z;
        this->vx = vx; this->vy = vy; this->vz = vz;
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
